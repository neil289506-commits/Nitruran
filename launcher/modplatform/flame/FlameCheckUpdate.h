#pragma once

#include "modplatform/CheckUpdateTask.h"

class FlameCheckUpdate : public CheckUpdateTask {
    Q_OBJECT

   public:
    FlameCheckUpdate(QList<Resource*>& resources,
                     std::vector<Version>& mcVersions,
                     QList<ModPlatform::ModLoaderType> loadersList,
                     ResourceFolderModel* resourceModel,
                     std::vector<ModPlatform::IndexedVersionType> releaseTypes = {})
        : CheckUpdateTask(resources, mcVersions, std::move(loadersList), resourceModel, std::move(releaseTypes))
    {}

   public slots:
    bool abort() override;

   protected slots:
    void executeTask() override;
   private slots:
    void getLatestVersionCallback(Resource* resource, QByteArray* response);
    void collectBlockedMods();
    void reportBlockedMods();
    void registerUpdate(Resource* resource, const ModPlatform::IndexedPack::Ptr& pack, const ModPlatform::IndexedVersion& latestVer);

   private:
    Task::Ptr m_task = nullptr;

    QHash<Resource*, QString> m_blocked;
    QHash<Resource*, std::pair<ModPlatform::IndexedPack::Ptr, ModPlatform::IndexedVersion>> m_blockedVersions;
    Task::Ptr m_mirrorTask;
};
