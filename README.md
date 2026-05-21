# BluFixer

Corretor de problemas de Bluetooth para Linux.

## Sobre

O BluFixer é uma ferramenta gráfica baseada em **GTK4** e **libadwaita** que diagnostica e corrige problemas comuns de Bluetooth em distribuições Linux. O aplicativo detecta adaptadores USB Bluetooth conectados e oferece correções específicas para o fabricante identificado, além de ações imediatas como liberação de antena e reinicialização do serviço.

## Funcionalidades

- **Varredura de dispositivos** — detecta adaptadores Bluetooth conectados via USB (usando `lsusb`)
- **Ficha técnica** — exibe nome do dispositivo, fabricante e ID de hardware
- **Seletor de tema** — alterna entre Claro, Escuro ou Seguir o Sistema; a preferência persiste entre sessões
- **Botão de atualização** — quando uma nova versão é detectada no GitHub, um botão aparece na barra de título
- **Site do projeto** — link direto para a página do GitHub no menu do aplicativo
- **Correções de sistema** (reversíveis):
  - Correção de energia para dongles CSR/Barrot e genéricos
  - Desativação de ERTM para hardwares como joysticks/gamepads
  - Pareamento em modo legado com PIN para dispositivos antigos
  - Instalação facilitada de firmware Realtek RTL8761B
  - Instalação facilitada de firmware Broadcom / Cypress (b43)
- **Ações imediatas**:
  - Descongestionamento de antena (rfkill)
  - Adição de permissões do usuário ao grupo lp
  - Limpeza completa de cache de dispositivos conectados
  - Reinicialização do serviço Bluetooth
- **Detecção multiplataforma**:
  - Privilégios: `pkexec` (padrão, com PolKit) ou `sudo -A` com askpass (`zenity`/`ssh-askpass`)
  - Download: `wget` ou `curl`
  - Serviço: `systemctl` ou `service`
  - Gerenciador de pacotes: `dnf`, `apt`, `zypper` ou `pacman`
  - Diretório de firmware: `/lib/firmware` ou `/usr/lib/firmware`
- **5 idiomas** — interface em Português, Inglês, Espanhol, Russo e Chinês
- **Código modular** — 9 módulos organizados em `src/` para manutenção facilitada

## Build recipes

### Dependências

- `gtk4`, `libadwaita`, `glib` (desenvolvimento)
- `pkg-config`
- Para Flatpak: `flatpak`, `flatpak-builder`
- Para RPM no Fedora: `rpm-build` (para `make rpm`)
- Para RPM com mock: `mock` (para `make rpm-mock`)

### Usando o Makefile

```bash
# Compilar
make

# Instalar localmente (em /usr/local)
sudo make install

# Desinstalar
sudo make uninstall

# Empacotar como tar.gz binário para distribuição (executável incluso)
make dist

# Construir pacote .deb (Debian / Ubuntu)
make deb

# Construir pacote RPM (Fedora / RHEL)
make rpm

# Construir RPM para Fedora 43 via mock
make rpm-mock

# Construir Flatpak
make flatpak

# Construir e gerar .flatpak bundle
make flatpak-bundle
```

### Receita para distribuição tar.gz (binário)

Desde a v1.6.0, o `make dist` gera um **tarball binário** com o executável
pré-compilado. O usuário pode executar diretamente sem precisar compilar.

```bash
# 1. Baixe o arquivo tar.gz em releases
https://github.com/mayrinck/blufixer/releases

# 2. Extrair no sistema de destino
tar -xzf BluFixer-1.6.0-x86_64.tar.gz
cd BluFixer-1.6.0

# 3. Executar diretamente (sem compilação)
./blufixer
```

Para instalação no sistema (a partir do tarball binário):

```bash
sudo install -m 0755 blufixer /usr/local/bin/
sudo install -m 0644 share/applications/*.desktop /usr/local/share/applications/
sudo install -m 0644 share/polkit-1/actions/*.policy /usr/local/share/polkit-1/actions/
sudo install -m 0644 share/icons/hicolor/*/apps/* /usr/local/share/icons/hicolor/*/apps/
gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor 2>/dev/null || true
```

### Receita para RPM (Fedora 43+)

```bash
# Construir o pacote RPM (gera .rpm e .src.rpm)
make rpm

# Instalar o RPM gerado
sudo dnf install build/BluFixer-1.6.0-x86_64.rpm

# Reconstruir para outra versão do Fedora a partir do SRPM:
# 1. Copie o SRPM (em ~/rpmbuild/SRPMS/) para a máquina de destino
# 2. Execute:
#    rpm --rebuild blufixer-1.6.0-*.src.rpm
```

**Compatibilidade entre versões do Fedora:**  
O SRPM (`make rpm`) pode ser reconstruído em qualquer versão do Fedora com
`rpm --rebuild`. Isso garante que as bibliotecas do sistema de destino
sejam usadas na linkedição. Para construir especificamente para Fedora 43
(ou outra versão usando mock):

```bash
# Instalar mock
sudo dnf install mock
sudo usermod -a -G mock $USER
# (reload session)

# Construir para Fedora 43 usando o SRPM gerado
make rpm-mock
```

O resultado será um RPM específico para Fedora 43 em `/var/lib/mock/fedora-43-x86_64/result/`.

### Receita para Flatpak

O projeto inclui um manifest Flatpak (`org.renanmayrinck.blufixer.yml`) para
build e bundle:

```bash
# 1. Adicionar Flathub e instalar o runtime GNOME
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.gnome.Platform//50 org.gnome.Sdk//50

# 2. Build e instalação local
make flatpak

# 3. Executar
flatpak run org.renanmayrinck.blufixer

# 4. (opcional) Gerar bundle .flatpak para distribuição offline
make flatpak-bundle
```

### Receita para .deb (Debian / Ubuntu)

O `Makefile` pode gerar um pacote `.deb` binário usando `dpkg-deb`:

```bash
# Construir o .deb
make deb

# Instalar
sudo dpkg -i build/BluFixer-1.6.0-amd64.deb
sudo apt install -f   # corrigir dependências, se necessário
```

**No Debian / Ubuntu nativamente:** o repositório inclui um diretório `debian/`
completo para uso com `dpkg-buildpackage`:

```bash
sudo apt build-dep .
dpkg-buildpackage -us -uc
```

## Licença

MIT License. Copyright © 2026 Renan Mayrinck.

## Autor

**Renan Mayrinck** — [renanmayrinck.com](https://www.renanmayrinck.com) — [Apoie via LiberaPay](https://liberapay.com/RenanMayrinck/)

### Aviso sobre uso de IA 🤖

Este projeto foi construido usando o **Opencode**, um gerenciador de inteligência artificial para programação. Apesar do projeto ser inteiramente funcional e verificado, é possível que o código não esteja na melhor estrutura ou siga os melhores padrões de qualidade usado por profissionais de programação. Toda ajuda é bem vinda para tornar o projeto mais profissiona, então se tiver sugestões de melhorias, por favor as envie como pull requests ou [adicione uma issue](https://github.com/mayrinck/blufixer/issues).
