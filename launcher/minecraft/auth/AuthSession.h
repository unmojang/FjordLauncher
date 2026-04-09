#pragma once

#include <QString>
#include <memory>

#include "LaunchMode.h"

class MinecraftAccount;

struct AuthSession {
    bool MakeOffline(QString offline_playername);
    void MakeDemo(QString name, QString uuid);

    QString serializeUserProperties();

    enum Status {
        Undetermined,
        RequiresOAuth,
        RequiresPassword,
        RequiresProfileSetup,
        PlayableOffline,
        PlayableOnline,
        GoneOrMigrated
    } status = Undetermined;

    // API URLs
    QString authlib_injector_url;
    QString auth_server_url;
    QString account_server_url;
    QString session_server_url;
    QString services_server_url;
    bool uses_custom_api_servers = false;
    QString authlib_injector_metadata;

    // account ID
    QString account_id;
    // client token
    QString client_token;
    // account user name
    QString username;
    // combined session ID
    QString session;
    // volatile auth token
    QString access_token;
    // profile name
    QString player_name;
    // profile ID
    QString uuid;
    // 'msa' or 'offline', depending on account type
    QString user_type;
    // the actual launch mode for this session
    LaunchMode launchMode;
};

using AuthSessionPtr = std::shared_ptr<AuthSession>;
