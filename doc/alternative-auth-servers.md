# Auth servers compatible with authlib-injector

## Public auth servers

<table>
    <thead>
        <tr>
            <td>Name/homepage</td>
            <td>Description</td>
            <td>API URL</td>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><a href="https://ely.by">Ely.by</a></td>
            <td>Licensed under <a href="https://github.com/elyby/accounts">Apache-2.0</a>, but not intended for self-hosting. If using 2FA, you must append the code to the end of your password during login in the format "password:code".</td>
            <td>https://account.ely.by/api/authlib-injector</td>
        </tr>
        <tr>
            <td><a href="https://skin.prinzeugen.net">Blessing Skin</a></td>
            <td>Also self-hostable.</td>
            <td>https://skin.prinzeugen.net/api/yggdrasil</td>
        </tr>
        <tr>
            <td><a href="https://littleskin.cn">LittleSkin</a></td>
            <td>Another popular instance of Blessing Skin.</td>
            <td>https://littleskin.cn/api/yggdrasil</td>
        </tr>
        <tr>
            <td><a href="https://drasl.unmojang.org">Drasl</a></td>
            <td>Also self-hostable. The public instance requires a Minecraft account to register.</td>
            <td>https://drasl.unmojang.org/authlib-injector</td>
        </tr>
    </tbody>
</table>

## Self-hosted API servers

<table>
    <thead>
        <tr>
            <td>Name/homepage</td>
            <td>Description</td>
            <td>License</td>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><a href="https://github.com/unmojang/drasl">Drasl</a></td>
            <td>Flexible, minimalistic API server written in Go</td>
            <td>GPLv3</td>
        </tr>
        <tr>
            <td><a href="https://github.com/bs-community/blessing-skin-server">Blessing Skin Server</a></td>
            <td>Feature-rich skin server written in PHP. Authentication server functionality is supported as a <a href="https://github.com/bs-community/blessing-skin-plugins">plugin</a>.</td>
            <td>MIT</td>
        </tr>
    </tbody>
</table>
