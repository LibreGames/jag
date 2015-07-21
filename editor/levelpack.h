#ifndef LEVELPACK_H
#define LEVELPACK_H

#include <QtGui/QDialog>
#include "ui_levelpack.h"

class LevelPackDialog : public QDialog {
    Q_OBJECT

public:
    LevelPackDialog(QWidget *parent = 0);

    int exec();

    QString packFileName()   { return ui.lePackFile->text(); }
    QString authorText()     { return ui.leAuthor->text(); }
    QString packNameText()   { return ui.lePackName->text(); }
    QString commentText()    { return ui.leComment->text(); }

    QStringList files();

private:
    Ui::LevelPackDialog ui;
    int m_result;
    QString author, packname, comment;

    void updateNavigation();
    void updateLevelsGroup();
    QString checkPathForDialog(const QString &path);

private slots:
    void on_bCancel_clicked()   { m_result = 0; close(); }
    void on_bNext_clicked();
    void on_bPrev_clicked();

    void on_bBrowse_clicked();
    void on_bUp_clicked();
    void on_bDown_clicked();
    void on_bRemove_clicked();
    void on_bClear_clicked();

    void on_bBrowsePackFile_clicked();
};

#endif // LEVELPACK_H
