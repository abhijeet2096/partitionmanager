/*************************************************************************
 *  Copyright (C) 2010 by Volker Lanz <vl@fidra.de>                      *
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

#if !defined(PROGRAMPAGEWIDGET_H)

#define PROGRAMPAGEWIDGET_H

#include "ui_configurepageprogram.h"

//#include <fs/filesystem.h>

#include <QWidget>

//class QString;
//class QComboBox;

class ProgramPageWidget : public QWidget, public Ui::ConfigurePageProgram
{
public:
    ProgramPageWidget(QWidget* parent);

//public:
//    QComboBox& comboBackend() {
//        return *m_ComboBackend;
//    }
//    const QComboBox& comboBackend() const {
//        return *m_ComboBackend;
//    }

//    QString backend() const;
//    void setBackend(const QString& name);

//private:
//    void setupDialog();
};

#endif

