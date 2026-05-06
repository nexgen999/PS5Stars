#include <ps5/ps5.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>

// On définit les chemins
#define CONFIG_PATH "/data/etaHEN/nfs.conf"
#define MOUNT_POINT "/data/etaHEN/games"

// Fonction pour lire l'IP dans le fichier nfs.conf
void get_nas_ip(char *ip) {
    FILE *f = fopen(CONFIG_PATH, "r");
    if (f) {
        fgets(ip, 16, f); // Lit la première ligne (l'IP)
        fclose(f);
    } else {
        strcpy(ip, "192.168.1.50"); // IP par défaut si pas de fichier
    }
}

// Démarrage du plugin
int module_start(size_t argc, const void *args) {
    char nas_ip[16];
    get_nas_ip(nas_ip);

    // Notification de début
    sceSysUtilSendSystemNotificationWithText(222, "Connexion au NAS...");

    // Commande de montage (Logique simplifiée pour le SDK)
    // On utilise nmount pour lier le partage réseau au dossier local
    struct iovec iov[6];
    iov[0].iov_base = "fstype"; iov[0].iov_len = 7;
    iov[1].iov_base = "nfs";    iov[1].iov_len = 4;
    iov[2].iov_base = "fspath"; iov[2].iov_len = 7;
    iov[3].iov_base = MOUNT_POINT; iov[3].iov_len = strlen(MOUNT_POINT) + 1;
    iov[4].iov_base = "from";   iov[4].iov_len = 5;
    
    char full_path[100];
    snprintf(full_path, sizeof(full_path), "%s:/volume1/jeups5", nas_ip);
    iov[5].iov_base = full_path; iov[5].iov_len = strlen(full_path) + 1;

    if (nmount(iov, 6, 0) < 0) {
        sceSysUtilSendSystemNotificationWithText(222, "Échec du montage NAS");
        return -1;
    }

    sceSysUtilSendSystemNotificationWithText(222, "NAS Monté avec succès !");
    return 0;
}

int module_stop(size_t argc, const void *args) {
    unmount(MOUNT_POINT, 0);
    return 0;
}
