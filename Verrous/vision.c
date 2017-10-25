#include <stdio.h>
#include <commun.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <mer.h>


float isModification(char * nomfic){//revoie 1 si il a ete modifie 0 sinon
	struct stat modif;

	stat(nomfic, &modif);

	if(time(NULL)-modif.st_mtime<1)
		return 1;
	return 0;
}

int isVerrou(int fd){

	struct flock verrou;

	verrou.l_whence = 0;
	verrou.l_start = 0;
	verrou.l_len = MER_TAILLE_ENTETE;


	fcntl(fd, F_GETLK, &verrou);

	if(verrou.l_type == F_RDLCK || verrou.l_type == F_WRLCK)
		return 1;
	return 0;

	
}	

int main( int nb_arg , char * tab_arg[] )
{
     char fich_mer[128] ;
     char nomprog[256] ;
 
     /*----------*/

     if( nb_arg != 2 )
     {
	 	fprintf( stderr , "Usage : %s <fichier mer>\n", tab_arg[0] );
	 	exit(-1);
     }

     strcpy( nomprog , tab_arg[0] );
     strcpy( fich_mer , tab_arg[1] );


     printf("\n%s : ----- Debut de l'affichage de la mer ----- \n", nomprog );
	
 		/***********/
    /* A FAIRE */
		/***********/

       int fd=1;
			 int nbBateaux=1;
			 int continuer=1;
       time_t derniereModif = time(NULL);


       mer_initialiser(fich_mer, 5, 5);

       if( (fd=open(fich_mer, O_RDONLY | O_CREAT, 0666))==-1)
       {
                exit(-1);
       }

       mer_nb_bateaux_lire( fd, &nbBateaux );
       mer_afficher(fd);

// Boucle tant qu'il y a plus de 2 bateaux
       while( 1 )
       {
// Si le fichier à été modifié il y a plus de 1 seconde sinon erreur
               if(isModification(fich_mer) && derniereModif!=time(NULL) )
               {
                      system("clear");
// On attend que le fichier mer soit deverrouiller
                       while(isVerrou(fd));

                       mer_afficher(fd);

// On verifie qu'il y a moins de deux bateaux
                       mer_nb_bateaux_lire( fd, &nbBateaux );
                       if(nbBateaux<2)
                                continuer=0;

// On met a jour la date de derniere modif
                       derniereModif=time(NULL);
               }

       }


       close(fd);

    /***********/
		

     printf("\n%s : --- Arret de l'affichage de la mer ---\n", nomprog );

     exit(0);
}
