#ifndef ICUPDATESYSTEM_H
#define ICUPDATESYSTEM_H

#include <QFrame>
#include <QTimer>

namespace Ui {
    class ICUpdateSystem;
}

class QSettings;

class ICUpdateSystem : public QFrame {
    Q_OBJECT
public:
    ICUpdateSystem(QWidget *parent = 0);
    ~ICUpdateSystem();

protected:
    void changeEvent(QEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);

private:
    void InitInterface();

private:
    Ui::ICUpdateSystem *ui;

    const QString updateIniPath_;
    const QString updateHostPath_;
    QSettings *updateSettings_;
    QSettings *updateHostSettings_;
    QTimer timer_;
    int status_;
    QDialog* updateDialog_;
//    QMap<int, int> hostStatusToStringMap_;
    bool connectHostFlag;

    void updateHostButton();
    void writeHostButton();
    void rebootButton();
    void RestartAndUpdateTheProgram();

private slots:
    void on_updateToolButton_clicked();
    void on_refreshToolButton_clicked();
    void SystemUpdateStart();
    void RefreshUSBIniInfo();
    void QueryStatus();

    void on_connectHostButton_clicked();
    void on_updateLogoButton_clicked();
};

//TODEBUG
//extern ICUpdateSystem *icUpdateSystem;

#endif // ICUPDATESYSTEM_H
