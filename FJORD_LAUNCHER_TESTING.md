# Fjord Launcher testing protocol

Before releasing a new version of Fjord Launcher, the following items should be manually tested to detect regressions. This protocol is not meant to test all of Fjord Launcher's functionality, just the features it adds over its upstream, Prism Launcher.

0. [ ] `rm -rf ~/.local/share/FjordLauncher`

1. Fetch CurseForge API key
    1. [ ] Skip through Fjord Launcher setup without adding a Microsoft account. On the CurseForge API key dialog, click "Yes".

2. DRM
    1. [ ] Settings &rarr; Accounts &rarr; "Add authlib-injector" should error
    2. [ ] Settings &rarr; Accounts &rarr; "Add Offline" should error

3. Microsoft accounts
    1. [ ] Settings &rarr; Accounts &rarr; "Add Microsoft" should work

4. authlib-injector accounts
    1. [ ] Settings &rarr; Accounts &rarr; "Add authlib-injector"
    2. [ ] Settings &rarr; Accounts &rarr; (select authlib-injector account) &rarr; Manage Skins
    3. [ ] Enter a valid player name who has a skin, click "Import user".
    4. [ ] Select a skin, click OK
    5. [ ] Reopen "Manage Skins", click "Reset Skin"

5. authlib-injector
    1. [ ] Launch latest Minecraft with an authlib-injector account. "No Yggdrasil agent is installed on this instance" dialog should show. Click "Install authlib-injector".
    2. [ ] Log in to an authlib-injector multiplayer server
    3. [ ] Edit instance &rarr; Version &rarr; select authlib-injector &rarr; Remove. Click "Add Agents", manually select an authlib-injector JAR, launch the instance, log into an authlib-injector multiplayer server
    4. [ ] Edit instance &rarr; Version &rarr; select authlib-injector &rarr; Remove.

6. Loki
    1. [ ] Launch latest Minecraft with an authlib-injector account. "No Yggdrasil agent is installed on this instance" dialog should show. Click "Install Loki".
    2. [ ] Log in to an authlib-injector multiplayer server
    3. [ ] Edit instance &rarr; Version &rarr; select Loki &rarr; Remove. Click "Add Agents", manually select a Loki JAR, launch the instance, log into an authlib-injector multiplayer server

7. About page
    1. [ ] Help &rarr; "About Fjord Launcher" should show correct launcher name and `$major.$minor.$patch.$downstream` release number
