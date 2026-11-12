// SPDX-License-Identifier: GPL-3.0-only
// Translations for Nitruran's own interface strings. Prism's translation files are downloaded at runtime
// and do not contain these strings, so they live here. Unknown language or missing key -> English.
#pragma once
#include <QHash>
#include <QLocale>
#include <QString>

#include "Application.h"
#include "settings/SettingsObject.h"

namespace Nitruran {

inline QString languageCode()
{
    QString lang = APPLICATION->settings()->get("Language").toString();
    if (lang.isEmpty())
        lang = QLocale::system().name();
    lang.replace('-', '_');
    if (lang.startsWith("zh_TW") || lang.startsWith("zh_HK") || lang.startsWith("zh_Hant"))
        return QStringLiteral("zh_TW");
    if (lang.startsWith("zh"))
        return QStringLiteral("zh_CN");
    return QStringLiteral("en");
}

inline QString t(const char* en)
{
    static const QHash<QString, QString> zhTW = {
        { "Home", "首頁" }, { "Discover", "探索" }, { "Library", "資源庫" }, { "Accounts", "帳號" }, { "Settings", "設定" },
        { "Sync", "同步" }, { "Welcome back!", "歡迎回來!" }, { "Jump back in", "繼續遊玩" }, { "New instance", "新增實例" },
        { "Play", "遊玩" }, { "Stop", "停止" }, { "Edit instance", "編輯實例" }, { "Played %1", "上次遊玩 %1" },
        { "Never played", "尚未遊玩" }, { "Discover and add instances", "探索並新增實例" },
        { "Sync from other launchers", "從其他啟動器同步" }, { "Detected launchers", "偵測到的啟動器" },
        { "Instances", "實例" }, { "Icons", "圖示" }, { "Sync now", "立即同步" }, { "Close", "關閉" },
        { "No data from Prism Launcher or mlcmLauncher was found.", "沒有找到 Prism Launcher 或 mlcmLauncher 的資料。" },
        { "%1 instances", "%1 個實例" }, { "Existing items are never overwritten.", "已存在的項目不會被覆蓋。" },
        { "Accounts from Prism Launcher may need to sign in again.", "Prism Launcher 的帳號可能需要重新登入。" },
        { "Sync finished", "同步完成" },
        { "Copied %1 instances, skipped %2 already present.", "已複製 %1 個實例,略過 %2 個已存在的實例。" },
        { "Accounts copied. Restart Nitruran to load them.", "帳號已複製,請重新啟動 Nitruran 以載入。" },
        { "Other launcher data found", "偵測到其他啟動器的資料" },
        { "Nitruran found data from %1. Import it now?", "Nitruran 偵測到 %1 的資料,要現在匯入嗎?" },
        { "Import", "匯入" }, { "Later", "稍後" }, { "Errors:", "錯誤:" }, { "Copying...", "複製中…" },
        { "Settings (memory, Java, language)", "設定(記憶體、Java、語言)" },
        { "Error", "錯誤" }, { "Warnings", "警告" },
        { "Your instance folder path contains characters that can break Java (e.g. '!' or ';'). Please move your instances to a different folder in the settings.", "你的實例資料夾路徑含有可能導致 Java 出問題的字元(例如 '!' 或 ';'),請到設定把實例移到其他資料夾。" },
    };
    static const QHash<QString, QString> zhCN = {
        { "Home", "主页" }, { "Discover", "发现" }, { "Library", "资源库" }, { "Accounts", "账号" }, { "Settings", "设置" },
        { "Sync", "同步" }, { "Welcome back!", "欢迎回来!" }, { "Jump back in", "继续游玩" }, { "New instance", "新建实例" },
        { "Play", "开始游戏" }, { "Stop", "停止" }, { "Edit instance", "编辑实例" }, { "Played %1", "上次游玩 %1" },
        { "Never played", "尚未游玩" }, { "Discover and add instances", "发现并添加实例" },
        { "Sync from other launchers", "从其他启动器同步" }, { "Detected launchers", "检测到的启动器" },
        { "Instances", "实例" }, { "Icons", "图标" }, { "Sync now", "立即同步" }, { "Close", "关闭" },
        { "No data from Prism Launcher or mlcmLauncher was found.", "没有找到 Prism Launcher 或 mlcmLauncher 的数据。" },
        { "%1 instances", "%1 个实例" }, { "Existing items are never overwritten.", "已存在的项目不会被覆盖。" },
        { "Accounts from Prism Launcher may need to sign in again.", "Prism Launcher 的账号可能需要重新登录。" },
        { "Sync finished", "同步完成" },
        { "Copied %1 instances, skipped %2 already present.", "已复制 %1 个实例,跳过 %2 个已存在的实例。" },
        { "Accounts copied. Restart Nitruran to load them.", "账号已复制,请重新启动 Nitruran 以加载。" },
        { "Other launcher data found", "检测到其他启动器的数据" },
        { "Nitruran found data from %1. Import it now?", "Nitruran 检测到 %1 的数据,现在导入吗?" },
        { "Import", "导入" }, { "Later", "稍后" }, { "Errors:", "错误:" }, { "Copying...", "复制中…" },
        { "Settings (memory, Java, language)", "设置(内存、Java、语言)" },
        { "Error", "错误" }, { "Warnings", "警告" },
        { "Your instance folder path contains characters that can break Java (e.g. '!' or ';'). Please move your instances to a different folder in the settings.", "你的实例文件夹路径含有可能导致 Java 出问题的字符(例如 '!' 或 ';'),请到设置把实例移到其他文件夹。" },
    };
    const QString key = QString::fromUtf8(en);
    const QString lang = languageCode();
    if (lang == "zh_TW")
        return zhTW.value(key, key);
    if (lang == "zh_CN")
        return zhCN.value(key, key);
    return key;
}

}  // namespace Nitruran
