# Comandos de Gestión de Grupos - Linux

## Crear grupo para control remoto total
```bash
sudo groupadd remote_control
```

## Crear grupo para visualización remota
```bash
sudo groupadd remote_view
```

## Verificar grupo remote_view
```bash
getent group remote_view
```

## Verificar grupo remote_control
```bash
getent group remote_control
```

## Ver usuario actual
```bash
whoami
```

## Ver información completa del usuario
```bash
id
```

## Agregar nuevo usuario
```bash
sudo adduser pruebaproyecto
```

## Asignar usuario al grupo remote_view
```bash
sudo usermod -a -G remote_view $USER
```

## Asignar usuario al grupo remote_control
```bash
sudo usermod -a -G remote_control $USER
```

## Ver grupos del usuario
```bash
groups $USER
```

## Remover usuario del grupo remote_view
```bash
sudo gpasswd -d $USER remote_view
```

## Remover usuario del grupo remote_control
```bash
sudo gpasswd -d $USER remote_control
```

## Ver todos los usuarios del sistema
```bash
cat /etc/passwd
```