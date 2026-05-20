# BluFixer

Corretor de problemas de Bluetooth para Linux.

## Sobre

O BluFixer é uma ferramenta gráfica baseada em **GTK4** e **libadwaita** que diagnostica e corrige problemas comuns de Bluetooth em distribuições Linux. O aplicativo detecta adaptadores USB Bluetooth conectados e oferece correções específicas para o fabricante identificado, além de ações imediatas como liberação de antena e reinicialização do serviço.

## Funcionalidades

- **Varredura de dispositivos** — detecta adaptadores Bluetooth conectados via USB (usando `lsusb`)
- **Ficha técnica** — exibe nome do dispositivo, fabricante e ID de hardware
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

## Build recipes

### Dependências

- `gtk4`, `libadwaita`, `glib` (desenvolvimento)
- `pkg-config`
- Para Flatpak: `flatpak`, `flatpak-builder`

### Usando o Makefile

```bash
# Compilar
make

# Instalar localmente (em /usr/local)
sudo make install

# Desinstalar
sudo make uninstall

# Empacotar como tar.gz para distribuição
make dist

# Construir Flatpak
make flatpak

# Construir e gerar .flatpak bundle
make flatpak-bundle
```

### Receita para distribuição tar.gz

```bash
# 1. Baixe o arquivo tar.gz em releases
https://github.com/mayrinck/blufixer/releases

# 2. Extrair no sistema de destino
tar -xzf org.renanmayrinck.blufixer-1.5.0.tar.gz
cd org.renanmayrinck.blufixer-1.5.0

# 3. Compilar e instalar
make
sudo make install
```

Para empacotadores: usar `DESTDIR` para instalar em diretório temporário.

```bash
make DESTDIR=/tmp/pkgdir PREFIX=/usr install
```

### Receita para RPM (Fedora / RHEL)

```bash
# Construir o pacote RPM (gera .rpm e .src.rpm)
make rpm

# Instalar o RPM gerado
sudo dnf install ~/rpmbuild/RPMS/x86_64/blufixer-*.rpm

# Reconstruir para outra versão do Fedora a partir do SRPM:
# 1. Copie o SRPM para a máquina de destino
# 2. Execute:
#    rpm --rebuild blufixer-1.5.0-*.src.rpm
```

**Compatibilidade entre versões do Fedora:**  
O SRPM (`make rpm`) pode ser reconstruído em qualquer versão do Fedora com
`rpm --rebuild`. Isso garante que as bibliotecas do sistema de destino
sejam usadas na linkedição. Para construir para Fedora 43 diretamente
do Fedora 44:

```bash
# Instalar mock
sudo dnf install mock
sudo usermod -a -G mock $USER
# (reload session)

# Construir para Fedora 43
make rpm-mock
```

### Receita para Flatpak

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

## Licença

MIT License. Copyright © 2026 Renan Mayrinck.

## Autor

**Renan Mayrinck** — [renanmayrinck.com](https://www.renanmayrinck.com) — [Apoie via LiberaPay](https://liberapay.com/RenanMayrinck/)
