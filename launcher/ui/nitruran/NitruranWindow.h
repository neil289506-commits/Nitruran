// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <QMainWindow>
#include <QMap>
#include <QUrl>

class QStackedWidget;
class QButtonGroup;
class QToolButton;
class Task;
class InstanceTask;

/**
 * Modrinth-App-style main window. Replaces ui/MainWindow as Application's main window;
 * all launcher logic still comes from Prism's core (Application, InstanceList, launch controller, accounts).
 */
class NitruranWindow : public QMainWindow {
    Q_OBJECT
   public:
    explicit NitruranWindow(QWidget* parent = nullptr);
    ~NitruranWindow() override;

    void checkInstancePathForProblems();
    void updatesAllowedChanged(bool allowed);
    void processURLs(QList<QUrl> urls);

   signals:
    void isClosing();

   protected:
    void closeEvent(QCloseEvent* event) override;

   private:
    void offerSyncOnFirstRun();
    QToolButton* addNavButton(const QString& glyph, const QString& label, const QString& tooltip, bool checkable);
    void addInstance(const QString& url = QString(), const QMap<QString, QString>& extraInfo = {});
    void runInstanceTask(InstanceTask* rawTask);
    void runModalTask(Task* task);

    QStackedWidget* m_pages = nullptr;
    QButtonGroup* m_navGroup = nullptr;
};
