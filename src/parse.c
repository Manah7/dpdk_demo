#include "parse.h"

/* Retourne une IP au format RTE depuis un (char *) */
rte_be32_t ip_from_string(char * sip){
    unsigned char bip[4];
    char *token = strtok(strdup(sip), ".");

    for (int j = 0; j < 4; ++j){
        bip[j] = atoi(token);
        token = strtok(NULL, ".");
    }

    return RTE_IPV4(bip[3], bip[2], bip[1], bip[0]);
}


/* Parse a given config. file 
 * La fonction est assez complexe pour pouvoir gérer quelques exceptions
 * élementaires. Des pointeurs de pointeurs sont utilisés pour pouvoir 
 * retourner des tableau de taille idéale et gagner en performance.
 */
int parse_config(char * filename, 
        int *nb_src_blk,
        rte_be32_t **deny_ip_src,
        int *nb_dst_blk,
        rte_be32_t **deny_ip_dst,
        int debug)
{
    /* Openning the file */
    if (debug){printf("Lecture du fichier : %s\n", filename);}
    FILE * cfp = fopen(filename, "r");
    if (cfp == NULL) 
        return -1;

    /* Compte du nombre de ligne (borne haute du nombre de règle) */
    char ch;
    int lines_count = 0;
    while(!feof(cfp)) {
        ch = fgetc(cfp);
        if(ch == '\n')
            lines_count++;
    }
    rewind(cfp);

    /* Variables temporaires */
    rte_be32_t * t_src_blk = malloc(lines_count * sizeof(rte_be32_t));
    rte_be32_t * t_dst_blk = malloc(lines_count * sizeof(rte_be32_t));

    /* Lecture ligne par ligne */
    char line[128];
    char s_blk[16], s_ins[16], s_ip4[16];
    while (fgets(line, sizeof(line), cfp)) {
        if (line[0] == '#'){
            if (debug){printf("Commentaire ignoré.\n");}
            continue;
        }

        /* Lecture de la ligne */
        sscanf(line, "%s %s %s\n", s_blk, s_ins, s_ip4);
        if (debug) {printf("Ligne : [%s][%s][%s]\n", s_blk, s_ins, s_ip4);}

        /* Vérification du premier mot-clé "block" et du format de ligne */
        if (strcmp(s_blk, "block")){
            if (strlen(line) > 1){
                if (debug){printf("Ligne inv.ld : \"%s\"\n", line);}
                return -1;
            } else {
                if (debug){printf("Ligne vide ignorée.\n");}
                continue;
            }
        }

        /* Interprétation du parsong */
        if (!strcmp(s_ins, "src")){
            t_src_blk[*nb_src_blk] = ip_from_string(s_ip4);
            (*nb_src_blk)++;
        } else if (!strcmp(s_ins, "dst")){
            t_dst_blk[*nb_dst_blk] = ip_from_string(s_ip4);
            (*nb_dst_blk)++;
        } else {
            if (debug){printf("L. inv.ld (mot clé inv.ld) : \"%s\"\n", line);}
            return -1;
        }

        /* Réinitialisation pour la prochaine boucle */
        s_blk[0] = '\0'; s_ins[0] = '\0'; s_ip4[0] = '\0';
    }

    /* Création des éléments finaux (gain de performance) */
    (*deny_ip_src) = malloc(*nb_src_blk * sizeof(rte_be32_t));
    for (int i = 0; i < *nb_src_blk; i++){
        (*deny_ip_src)[i] = t_src_blk[i];
    }
    (*deny_ip_dst) = malloc(*nb_dst_blk * sizeof(rte_be32_t));
    for (int i = 0; i < *nb_dst_blk; i++){
        (*deny_ip_dst)[i] = t_dst_blk[i];
    }

    /* Nettoyage */
    free(t_src_blk);
    free(t_dst_blk);
    fclose(cfp);

    return 0;
}
