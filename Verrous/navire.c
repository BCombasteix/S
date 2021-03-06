#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> /* fcntl */
#include <unistd.h>
#include <fcntl.h>
#include <errno.h> /* errno */
#include <string.h>

#include <bateau.h>
#include <mer.h>

void BateauDetruire(int fd, int * nbBateaux, bateau_t * bateau) {
	mer_nb_bateaux_ecrire(fd, --(*nbBateaux));
	bateau_destroy(&bateau);
}


int VerrMer(const int fd, bateau_t * bateau, int verrouiller){
	struct flock verrou;

	verrou.l_whence = 0;
	verrou.l_start = 0;
	verrou.l_len = 0;

	if(verrouiller){
		verrou.l_type = F_WRLCK;
	} else {
		verrou.l_type = F_UNLCK;
	
	}
	
	if(fcntl(fd, F_SETLK, &verrou) == ERREUR) {
		if(verrouiller) {
			printf("Erreur lors de la pose du verrou ecrivain\n");
		} else {
			printf("Erreur lors de la levee du verrou ecrivain\n");
		}
		
		return ERREUR;
	}
	
	return CORRECT;
}
	
int VerrBouclier(const int fd, bateau_t * bateau, int verrouiller) {
// Verrouille les cellules appartenant au bateau
	int i;
	coords_t * cell1 = bateau_corps_get(bateau);
	coord_t * cell2 = cell1->coords;
	
	struct flock verrou;
	
	verrou.l_whence = 0;
	verrou.l_len = MER_TAILLE_CASE;
	
	if(verrouiller) {
		verrou.l_type = F_WRLCK;
	} else {
		verrou.l_type = F_UNLCK;
	}
	
	for(i = 0; i < cell1->nb; i++) {
		verrou.l_start = cell2[i].pos;
	
		if(fcntl(fd, F_SETLK, &verrou) == ERREUR) {
			if(verrouiller) {
				printf("Erreur lors de la pose du verrou bouclier\n");
			} else {
				printf("Erreur lors de la levee du verrou bouclier\n");
			}
			
			return ERREUR;
		}
	}
	if(verrouiller)
		printf("Bateau  %c bouclier pose \n",bateau_marque_get(bateau));
	else
		printf("Bateau  %c bouclier levee \n",bateau_marque_get(bateau));

	return CORRECT;
}

int VerrHead(const int fd, bateau_t * bateau, int verrouiller) {

	struct flock verrou;
	
	verrou.l_whence = 0;
	verrou.l_start = 0;
	verrou.l_len = MER_TAILLE_ENTETE;
	
	if(verrouiller) {
		verrou.l_type = F_WRLCK;
	} else {
		verrou.l_type = F_UNLCK;
	}
	
	if(fcntl(fd, F_SETLK, &verrou) == -1) {
// erreur si il y a deja un verrou
		return ERREUR;
		printf("erreur verrou header\n");
	}
	return CORRECT;
}

int VerrVoisin(const int fd, bateau_t * bateau, const coords_t listeVoisins, int verrouiller) {
	int i;
	case_t mer_cell;
	struct flock verrou;
	
	verrou.l_whence = 0;
	verrou.l_len = 0;
	
	if(verrouiller) {
		verrou.l_type = F_WRLCK;
	} else {
		verrou.l_type = F_UNLCK;
	}
	
	for(i = 0; i < listeVoisins.nb; i++) {
		mer_case_lire(fd, listeVoisins.coords[i], &mer_cell);
		if(mer_cell == MER_CASE_LIBRE) { // Si la case est libre, on la verrouille
			verrou.l_start = listeVoisins.coords[i].pos;
	
			if(fcntl(fd, F_SETLK, &verrou) == -1) {
				if(verrouiller) {
					printf("Erreur lors de la pose du verrou pour les voisins\n");
				} else {
					printf("Erreur lors de la levee du verrou pour les voisins\n");
				}
				
				return ERREUR;
			}
		}
	}
	
	return CORRECT;
}

int VerrCible(const int fd, bateau_t * bateau, coord_t cible, int verrouiller) {
	// Pose un verrou sur une case ou les coordonnes sont en parametres
	struct flock verrou;
	
	verrou.l_whence = 0;
	verrou.l_start = cible.pos;
	verrou.l_len = MER_TAILLE_CASE;
	
	if(verrouiller) {
		verrou.l_type = F_WRLCK;
	} else {
		verrou.l_type = F_UNLCK;
	}
	
	
	
	if(fcntl(fd, F_SETLK, &verrou)) {
		if(verrouiller) {
			printf("Erreur lors de la pose du verrou cible\n");
		} else {
			printf("Erreur lors de la levee du verrou cible\n");
		}
		
		return ERREUR;
	}
	
	return CORRECT;
}


/* 
 *  Programme principal 
 */

int
main( int nb_arg , char * tab_arg[] )
{
  char fich_mer[128] ;
  case_t marque = MER_CASE_LIBRE ;
  char nomprog[128] ;
  float energie = 0.0 ;

  /*----------*/

  strcpy( nomprog , tab_arg[0] ) ;

  if( nb_arg != 4 )
    {
      fprintf( stderr , "Usage : %s <fichier mer> <symbole> <energie>\n", 
	       nomprog );
      exit(-1);
    }
//barriere anti personne qui mets plus d'un element
  if( strlen(tab_arg[2]) !=1 ) 
    {
      fprintf( stderr , "%s : erreur symbole <%s> incorrecte, ne mettre q'un element\n",
	       nomprog , tab_arg[2] );
      exit(-1) ;
    }


  strcpy( fich_mer , tab_arg[1] );
  marque = tab_arg[2][0] ;
  sscanf( tab_arg[3] , "%f" , &energie );
  
  /* Initialisation de la generation des nombres pseudo-aleatoires */
  srandom((unsigned int)getpid());

  printf( "%s : ----- Debut du bateau %c (%d) -----\n ", 
	  nomprog , marque , getpid() );

  /***********/
  /* A FAIRE */
  /***********/
  bateau_t * bateau = BATEAU_NULL ;
	int nbBateaux = 0 ; 
	int fd;								
	int no_error;
	
	if( ( fd = open( fich_mer , O_RDWR | O_CREAT , 0666)) == -1 ) {
		printf("Erreur dans l'ouverture du fichier mer");
		return(ERREUR);
	}
	
	bateau	= bateau_new(NULL, marque, energie);
	
//init bateau
	if((no_error = VerrMer(fd, bateau, 1))) {
		printf("Erreur dans le verrouillage de la mer\n");
		
		exit(no_error);
	}
	
	if((no_error = mer_bateau_initialiser(fd, bateau))) {
		printf("Erreur dans l'initialisation de la mer\n");
		BateauDetruire(fd,&nbBateaux, bateau);
		
		exit(no_error);
	}
	
	mer_nb_bateaux_lire(fd, &nbBateaux);
	mer_nb_bateaux_ecrire(fd, ++nbBateaux);

	VerrMer(fd, bateau, 0);

//attente que tout les bateaux soit placer
	sleep(5);
	
//init shield
	if(energie >= BATEAU_SEUIL_BOUCLIER) {
		if((no_error = VerrBouclier(fd, bateau, 1))) {
			printf("Erreur lors du placement du bouclier\n");

			exit(no_error);
		}
	}
	
//attente de tout les boucliers
	sleep(5);
	

	booleen_t touche = FAUX;
	booleen_t deplacement = VRAI;
	booleen_t acquisition;
	coords_t * listeVoisins = NULL;
	coord_t cible;

	char bateauMarque;
		
	while(1) {
		sleep(3); // Entre 1 et 3 secondes
		
		bateauMarque = bateau_marque_get(bateau);
		
		if((no_error = mer_bateau_est_touche(fd, bateau, &touche))) {
//destruction du bateau si prob
			printf("Erreur dans la vérification de l'attaque sur un bateau\n");
			BateauDetruire(fd,&nbBateaux, bateau);
			
			exit(no_error);
		}
		
		mer_nb_bateaux_lire(fd, &nbBateaux);
		
		if(touche && energie < BATEAU_SEUIL_BOUCLIER) {
//destruction du bateau si touche
			printf(" Bateau %c : Destruction \n", bateauMarque);
			mer_nb_bateaux_ecrire(fd, --nbBateaux);
			mer_bateau_couler(fd, bateau);
			bateau_destroy(&bateau);
			close(fd);
			
			exit(0);
		}
				
		if(nbBateaux == 1) {
//message fin
			printf("  Bateau %c : Vainqueur \n", bateauMarque);
			printf("  Bateau %c : Fin \n", bateauMarque);
			sleep(1);
		
			close(fd);
			
			exit(0);

		}
		
		listeVoisins = coords_new();
		if((no_error = mer_voisins_rechercher(fd, bateau, &listeVoisins))) {
			printf("Erreur lors de la recherche de voisins\n");
			BateauDetruire(fd,&nbBateaux, bateau);
			
			exit(no_error);
		}
		
//deplacement bateau
		if(energie > 0) {
			if(VerrVoisin(fd, bateau, *listeVoisins, 1) == ERREUR) {
				printf("Erreur lors du deplacement du bateau %c\n", marque);
				deplacement = FAUX;
			} else {
				if((no_error = VerrBouclier(fd, bateau, 0))) {
					printf("Erreur lors de la desactivation du bouclier\n");
					BateauDetruire(fd,&nbBateaux, bateau);
				
					exit(no_error);
				}
			}
		
//on ferme la lecture
			VerrHead(fd, bateau, 1);

		
			if((no_error = mer_bateau_deplacer(fd, bateau, listeVoisins, &deplacement))) {
				printf("Erreur dans le deplacement du bateau sur la mer\n");
				BateauDetruire(fd,&nbBateaux, bateau);
			
				exit(no_error);
			} else {
				energie -= BATEAU_MAX_ENERGIE * 0.05;
			}

//on ouvre la lecture		
			VerrHead(fd, bateau, 0);

		}
		
		printf(" Bateau %c : %f d'energie restante \n", bateauMarque, energie);
		
		if(energie >= BATEAU_SEUIL_BOUCLIER) {
			// On verifie si l'energie du bateau est suffisante pour reactiver les boucliers
			if((no_error = VerrBouclier(fd, bateau, 1))) {
				printf("Erreur lors de l'activation du bouclier\n");
				BateauDetruire(fd,&nbBateaux, bateau);
				
				exit(no_error);
			}
		}
		
		if(VerrVoisin(fd, bateau, *listeVoisins, 0) == ERREUR) {
			printf("Erreur lors du deverouillage des cases adjacentes au bateau %c", marque);
			BateauDetruire(fd,&nbBateaux, bateau);
			
			exit(no_error);
		}
		
		if(deplacement) {
			printf(" Deplacement effectuer\n");
		}
		else {
			printf(" Deplacement annule\n");
		}
	
		coords_destroy(&listeVoisins);
		
//aquisition du bateau
		 
		if((no_error = mer_bateau_cible_acquerir(fd, bateau, &acquisition, &cible))) {
			printf("Erreur lors de l'acquisition d'un bateau");
			BateauDetruire(fd,&nbBateaux, bateau);
	
			exit(no_error);
		}
		 
		if(acquisition) { 
			if(VerrCible(fd, bateau, cible, 1) != ERREUR) { 
				VerrCible(fd, bateau, cible, 0);
				if((no_error = mer_bateau_cible_tirer(fd, cible))) {
					printf("Erreur lors du tir du bateau %c\n", marque);
					BateauDetruire(fd,&nbBateaux, bateau);
					 
					exit(no_error);
				}
			
				if(VerrCible(fd, bateau, cible, 0) == ERREUR) {
					printf("Erreur lors du deverouillage du bateau %c\n", marque);
					BateauDetruire(fd,&nbBateaux, bateau);
					 
					exit(no_error);
				}
			} else {
				printf("Erreur lors du verouillage de la cible en [%i, %i]\n", cible.l, cible.c);
			}
		} else {
			printf("Erreur lors de l'acquisition d'une cible pour le bateau %c\n", marque);
		}
		
		printf("\n");
	}
	
	

	printf( "%s : ----- Fin du navire %c (%d) -----\n ", 
	nomprog , marque , getpid() );

	exit(0);
}
