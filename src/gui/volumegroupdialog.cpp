/*************************************************************************
 *  Copyright (C) 2016 by Chantara Tith <tith.chantara@gmail.com>        *
 *  Copyright (C) 2016 by Andrius Štikonas <andrius@stikonas.eu>         *
 *                                                                       *
 *  This program is free software; you can redistribute it and/or        *
 *  modify it under the terms of the GNU General Public License as       *
 *  published by the Free Software Foundation; either version 3 of       *
 *  the License, or (at your option) any later version.                  *
 *                                                                       *
 *  This program is distributed in the hope that it will be useful,      *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *  GNU General Public License for more details.                         *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 *************************************************************************/

#include "gui/volumegroupdialog.h"
#include "gui/volumegroupwidget.h"

#include <core/partitiontable.h>
#include <core/lvmdevice.h>
#include <fs/lvm2_pv.h>

#include <util/capacity.h>
#include <util/helpers.h>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <QtGlobal>
#include <QListWidgetItem>
#include <QRegularExpressionValidator>

/** Creates a new VolumeGroupDialog
    @param parent pointer to the parent widget
    @param vgName Volume Group name
    @param pvList List of LVM Physical Volumes used to create Volume Group
*/
VolumeGroupDialog::VolumeGroupDialog(QWidget* parent, QString& vgName, QVector<const Partition*>& pvList) :
    QDialog(parent),
    m_DialogWidget(new VolumeGroupWidget(this)),
    m_TargetName(vgName),
    m_TargetPVList(pvList),
    m_IsValidSize(false),
    m_IsValidName(true),
    m_TotalSize(0),
    m_TotalUsedSize(0),
    m_ExtentSize(0)
{
    mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->addWidget(&dialogWidget());

    dialogButtonBox = new QDialogButtonBox;
    okButton = dialogButtonBox->addButton(QDialogButtonBox::Ok);
    cancelButton = dialogButtonBox->addButton(QDialogButtonBox::Cancel);
    mainLayout->addWidget(dialogButtonBox);

    cancelButton->setFocus();
    cancelButton->setDefault(true);

    setupDialog();
    setupConstraints();
    setupConnections();
}

/** Destroys a VolumeGroupDialog */
VolumeGroupDialog::~VolumeGroupDialog()
{
    KConfigGroup kcg(KSharedConfig::openConfig(), "createVolumeGroupDialog");
    kcg.writeEntry("Geometry", saveGeometry());
}

void VolumeGroupDialog::setupDialog()
{
    /* LVM Volume group name can consist of: letters numbers _ . - +
     * It cannot start with underscore _ and must not be equal to . or .. or any entry in /dev/
     * QLineEdit accepts QValidator::Intermediate, so we just disable . at the beginning */
    QRegularExpression re(QStringLiteral(R"(^(?!_|\.)[\w\-.+]+)"));
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(re, this);
    dialogWidget().vgName().setValidator(validator);
    dialogWidget().vgName().setText(targetName());

    dialogWidget().volumeType().addItem(QStringLiteral("LVM"));
    dialogWidget().volumeType().addItem(QStringLiteral("RAID"));
    dialogWidget().volumeType().setCurrentIndex(0);

    setMinimumSize(dialogWidget().size());
    resize(dialogWidget().size());
}

void VolumeGroupDialog::setupConnections()
{
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &VolumeGroupDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &VolumeGroupDialog::reject);
    connect(&dialogWidget().volumeType(), qOverload<int>(&QComboBox::currentIndexChanged), this, &VolumeGroupDialog::onVolumeTypeChanged);
    connect(&dialogWidget().listPV().listPhysicalVolumes(), &QListWidget::itemChanged,
            this, [=] ( QListWidgetItem*) {
                updateSizeInfos();
            });
}

void VolumeGroupDialog::setupConstraints()
{
    updateSizeInfos();
    updateOkButtonStatus();
}

void VolumeGroupDialog::updateOkButtonStatus()
{
    bool enable = isValidSize();

    if (dialogWidget().vgName().text().isEmpty() || !isValidName()) {
        enable = false;
    }
    if (dialogWidget().spinPESize().value() <= 0) {
        enable = false;
    }

    okButton->setEnabled(enable);
}

void VolumeGroupDialog::updateSectorInfos()
{
    qint32 totalSectors = 0;
    // we can't use LvmDevice mothod here because pv that is not in any VG will return 0
    m_ExtentSize = dialogWidget().spinPESize().value() * Capacity::unitFactor(Capacity::Byte, Capacity::MiB);
    if (m_ExtentSize > 0) {
        totalSectors = m_TotalSize / m_ExtentSize;
    }
    dialogWidget().totalSectors().setText(QString::number(totalSectors));
}

void VolumeGroupDialog::updateSizeInfos()
{
    const QVector<const Partition *> checkedPartitions = dialogWidget().listPV().checkedItems();
    m_TotalSize = 0;
    for (const auto &p : checkedPartitions)
        m_TotalSize += p->capacity() - p->capacity() % (dialogWidget().spinPESize().value() * Capacity::unitFactor(Capacity::Byte, Capacity::MiB)); // subtract space which is too small to hold PE

    dialogWidget().totalSize().setText(Capacity::formatByteSize(m_TotalSize));

    //Probably a bad design for updating state here; the state should be changed inside the update button function.
    m_IsValidSize = m_TotalSize >= m_TotalUsedSize;
    updateSectorInfos();
    updateOkButtonStatus();
}

void VolumeGroupDialog::updatePartitionList()
{
}

void VolumeGroupDialog::onPartitionListChanged()
{
}

void VolumeGroupDialog::onVolumeTypeChanged(int index)
{
    Q_UNUSED(index)
    updatePartitionList();
}
