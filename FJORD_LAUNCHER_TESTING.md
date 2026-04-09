# Fjord Launcher testing protocol

Before releasing a new version of Fjord Launcher, the following items should be manually tested to detect regressions. This protocol is not meant to test all of Fjord Launcher's functionality, just the features it adds over its upstream, Prism Launcher.

0. [ ] `rm -rf ~/.local/share/FjordLauncher`

1. Fetch CurseForge API key

    a. [ ] Skip through Fjord Launcher setup without adding a Microsoft account. On the CurseForge API key dialog, click "Yes".

2. DRM

    a. [ ] Settings &rarr; Accounts &rarr; "Add authlib-injector" should error
    b. [ ] Settings &rarr; Accounts &rarr; "Add Offline" should error

3. Microsoft accounts

    a. [ ] Settings &rarr; Accounts &rarr; "Add Microsoft" should work

4. authlib-injector accounts

    a. [ ] Settings &rarr; Accounts &rarr; "Add authlib-injector"
    b. [ ] Settings &rarr; Accounts &rarr; (select authlib-injector account) &rarr; Manage Skins
    c. [ ] Enter a valid player name who has a skin, click "Import user".
    d. [ ] Select a skin, click OK
    e. [ ] Reopen "Manage Skins", click "Reset Skin"

5. authlib-injector

    a. [ ] Launch latest Minecraft with an authlib-injector account. "authlib-injector is not installed" dialog should show. Click "Yes".
    b. [ ] Log in to an authlib-injector multiplayer server
    c. [ ] Edit instance &rarr; Version &rarr; select authlib-injector &rarr; Remove. Click "Add Agents", manually select an authlib-injector JAR, launch the instance, log into an authlib-injector multiplayer server

6. About page

    a. [ ] Help &rarr; "About Fjord Launcher" should show correct launcher name and `$major.$minor.$patch.$downstream` release number
