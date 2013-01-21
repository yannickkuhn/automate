#include <stdlib.h>
#include <stdio.h>
#include <fmod.h>
#include <SDL/SDL.h>
#define NBCARMAX 6+1		// le train compte pour 1

#define LARGEUR_TILE 128
#define HAUTEUR_TILE 96
#define LARGEUR_FENETRE 896
#define HAUTEUR_FENETRE 576

// Ce code du 13/01/2012 [3] permet d'afficher deux voitures et de les faire bouger ...
// - amelioration (julien) : remplacer des carrés rouges par des voitures
// - amelioration (julien) : ajout du train
// - amelioration (julien) : création et mise en place de la map d'affichage
// - amelioration (julien) : mis en place du train et des feux rouges pour les voitures,
//								15/01/2013 -> reunion ou on s'est vu pour discuter des difficultés
//								mis en place ensemble de la synchonisation pour que les voitures
//								s'arretent aux feux rouges quand le train passe

// CODE AU 17/01/2012 - Changements Effectués ...
// - amelioration (yannick) : changement de certaines variables pour mieux se repérer dans le code
// - amelioration (yannick) : generation des voitures en automatique et tests

// ---------------------
// ATTENTION : Les fonctions "affichent" et "actualiseAffichageVoiture" commencent à l'indice 1 (les boucles) à cause du train
// voir si une amélioration est à envisager ...
// ---------------------

// il reste aussi les voitures à faire arrêter aux feux ! Voir quel code est à modifier ! Sachant qu'il est déjà intégré chez Julien !
// INDICATION : voir ligne 120 (environ) : gestion du feu stop pour les voitures

// if(testDuPassageDuTrain(&car[0])==1)
//       	car[0].etat = 1;
// else
//        	car[0].etat = 0;


// - RESTE A FAIRE
//   _____________
//
// - gestion avec les fichiers (enregistrement et lecture)
// 			demander au prof pour l'association (liaison directe) cellules/map dans le cadre d'un bouchon autoroutier
// - gérer le fait que les voitures doivent s'arrêter les unes derrière les autres
// - pour l'instant, dans le code "genereVoiture",
//				il y a (ligne 190 environ) 'setParamVoiture(&voit, 2, 0, 230+voie*90);'
//				en temps normal il faudrait faire : 'setParamVoiture(&voit, vitesse, 0, 230+voie*90);' : vitesse variable (générée aléatoirement)
//				Ce qu'il faut faire, c'est effectivement modifier le paramètre puis gérer en amont les collisions entre véhicule (différences de vitesse d'une voiture qui arrive trop vite derrière l'autre à gérer par exemple)
// - AUTRES IDEES
//   ____________
//
// - ajout d'un indice pour densifier/diminuer le traffic ( à voir )
// - gérer les ajout/modifications des cellules manuelles (gestion du menu, avec quitter puis toutes les options) - voir sujet
// - ajout du feu orange (peut etre)
// - gérer correctement le train et la voiture pour faire une vrai gestion de cellules (je me réserve cette partie [YK])
// - tests unitaires, pour le rapport ?! (partie réservée à YK)
//   ____________

typedef struct voiture {
	char etat;
	SDL_Rect pos;
	int vitesse;
} T_VOIT;

char* table[] = {
"0012344",
"5567BCL",
"::;<=::",
"HH@AHHH",
"KK89EKK",
"GGIJFGG",};

void afficherMapEtTrain(SDL_Surface* screen,SDL_Surface* tileset,char** table,int nombre_blocs_largeur,int nombre_blocs_hauteur) {

	int i,j;

	SDL_Rect Rect_dest;
	SDL_Rect Rect_source;
	Rect_source.w = LARGEUR_TILE;
	Rect_source.h = HAUTEUR_TILE;
	for(i=0;i<nombre_blocs_largeur;i++)
	{
		for(j=0;j<nombre_blocs_hauteur;j++)
		{
			Rect_dest.x = i*LARGEUR_TILE;
			Rect_dest.y = j*HAUTEUR_TILE;
			Rect_source.x = (table[j][i]-'0')*LARGEUR_TILE;
			Rect_source.y = 0;
			SDL_BlitSurface(tileset,&Rect_source,screen,&Rect_dest);
		}
	}

}

void afficherFeuRouge(SDL_Surface* screen,SDL_Surface* tileset,char** table,int nombre_blocs_largeur,int nombre_blocs_hauteur,int arret) {

	SDL_Rect Rect_dest;
	SDL_Rect Rect_source;
	Rect_source.w = LARGEUR_TILE;
	Rect_source.h = HAUTEUR_TILE;
	Rect_dest.x = 2*LARGEUR_TILE;
	Rect_dest.y = 1*HAUTEUR_TILE;
	Rect_source.y = 0;
	      if(arret==0)
	      {
			Rect_source.x = ('>'-'0')*LARGEUR_TILE;
	      }
	      else
	      {
	       	Rect_source.x = ('6'-'0')*LARGEUR_TILE;
	      }

			SDL_BlitSurface(tileset,&Rect_source,screen,&Rect_dest);
}

void afficherVoitures(T_VOIT *voit, int nbVoitures, SDL_Surface *img, SDL_Surface *ecran) {
	int i;

	// gestion des collisions à rajouter ultérieurement !

	// gestion du feu stop pour les voitures (lorsque le train passe)
	int idTrain = 0;
	for(i=1;i<nbVoitures;i++) {

		if(voit[0].etat == 1) {		// si le train impose le feu rouge (circule sur la voie)
			if(voit[i].pos.x >= 180 && voit[i].pos.x < 200) {		// les voitures, lorsqu'elles arrivent entre la pos 180|200, s'arretent
				voit[i].etat = 1;
			}
		}
		else {						// si le train est reparti, les voitures peuvent repartir ...
			if(voit[i].etat == 1)
				voit[i].etat = 0;
		}

		// reactualisation de l'affichage des voitures
		SDL_BlitSurface(img, NULL, ecran, &voit[i].pos);
	}
}

Uint32 actualisePosVoitures(Uint32 intervalle, void *parametre) {
	T_VOIT *voit;
	int i;
	voit = parametre;
	for(i=1;i<NBCARMAX;i++) {
		if(voit[i].etat == 0) {		// si la voiture peut avancée
			voit[i].pos.x = voit[i].pos.x+voit[i].vitesse;
			if(voit[i].pos.x > 850)
				voit[i].pos.x = -100;
		}
	}
	return intervalle;
}

Uint32 actualisePosTrain(Uint32 intervalle, void *parametre) {
	T_VOIT *voit;
	voit = parametre;
    voit->pos.y = voit->pos.y+voit->vitesse;
	if(voit->pos.y > 1200)
		voit->pos.y = -300;
	return intervalle;

}

void setParamVoiture(T_VOIT *vehicule, int vitesse, int posH, int posV) {
	vehicule->vitesse = vitesse;
	vehicule->pos.x = posH;
	vehicule->pos.y = posV;
	vehicule->etat = 0;
}

void affichageParamVoiture(T_VOIT vehicule) {
	printf("\nVoici les caractéristiques de celui ci : position : %d %d, vitesse : %d\n",vehicule.pos.x, vehicule.pos.y, vehicule.vitesse);
}

int testDuPassageDuTrain(T_VOIT *vehicule) {
	if((vehicule->pos.y>-300)&&(vehicule->pos.y<500))
		return 1;
	else
		return 0;
}

int PresenceVoituresEnPosNulle(T_VOIT *voit, int nbVoitures, int posy) { 		// pour pas qu'une voiture soit généré sur une position où il y a déjà une autre voiture
	int i;
	for(i=0;i<nbVoitures;i++) {
		if(voit[i].pos.x >= 0 && voit[i].pos.x < 150 && voit[i].pos.y == posy) {
			return 1;
		}
	}
	return 0;
}

T_VOIT genereVoiture() {	/* ne générer une voiture que si la position en x est libre */
	T_VOIT voit;
	int voie = (int)(rand() / (double)RAND_MAX * (3 - 1));			// voie sera soit 0 ou 1 (3-1) = 2 possibilités
	int vitesse = (int)(rand() / (double)RAND_MAX * (7 - 1)) + 1;	// vitesse entre 1 et 6 (7-1)+1 -> 6 possibilités, +1 pour que ça commence à l'indice '1'
	setParamVoiture(&voit, 2, 0, 230+voie*90);
	return voit;
}

/* CODE DU PROGRAMME PRINCIPAL */

int main(int argc, char *argv[])
{
	// Variables
	SDL_Surface *ecran = NULL, *imgFond = NULL, *imgCar = NULL, *tileset = NULL, *imgTrain = NULL;
	SDL_Rect pFond;
	SDL_Event event;
	T_VOIT car[NBCARMAX], cartmp;
	int continuer;
	int nbVoitures;
	int flag;
	int flag1;
	flag=0;
	flag1=0;

	// Variables du Timer
	SDL_TimerID idTimerTrain, idTimerCar;
	int periode;

	// initialisation
	continuer = 1;
	periode = 10;

	setParamVoiture(&car[0],1,358,0);		// car[0] -> le train
	affichageParamVoiture(car[0]);						// le train
	nbVoitures = 1;

	tileset = SDL_LoadBMP("route.bmp");

	//initialisation fmod
	FMOD_SYSTEM *system;
    FMOD_System_Create(&system);
    FMOD_System_Init(system, 2, FMOD_INIT_NORMAL, NULL);
    FMOD_SOUND *circulation = NULL,*train = NULL;
    FMOD_System_CreateSound(system, "circulation.wav", FMOD_CREATESAMPLE, 0, &circulation);
    FMOD_System_CreateSound(system, "train.wav", FMOD_CREATESAMPLE, 0, &train);
	// essai en initialisant une voiture
	//car[nbVoitures] = genereVoiture();
	//nbVoitures++;

	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	idTimerTrain = SDL_AddTimer(periode,actualisePosTrain,&car[0]);
	idTimerCar = SDL_AddTimer(periode,actualisePosVoitures,car);
	ecran = SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Premiere collision de vehicules ...", NULL);
	imgCar = SDL_LoadBMP("voiture.bmp");
	imgTrain = SDL_LoadBMP("train.bmp");
	afficherMapEtTrain(ecran,tileset,table,LARGEUR_FENETRE/LARGEUR_TILE,HAUTEUR_FENETRE/HAUTEUR_TILE);
	SDL_BlitSurface(imgTrain, NULL, ecran, &car[0].pos);		// le train (actualise son affichage)
	SDL_Flip(ecran);

	while (continuer) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					continuer = 0;
					break;
			}
		}

		if(nbVoitures<NBCARMAX) {
			cartmp = genereVoiture();
			if(PresenceVoituresEnPosNulle(car,nbVoitures,cartmp.pos.y) == 0) {
				car[nbVoitures] = cartmp;
				nbVoitures++;
			}
		}

        if(testDuPassageDuTrain(&car[0])==1)
        {
            if(flag==0)
            {
             flag=1;
             FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, train, 0, NULL);
            }
            flag1=0;
        	car[0].etat = 1;	// declenche un feu rouge pour les voitures
        }
        else
        {
            flag=0;
            car[0].etat = 0;
            if (flag1==0)
            {
              flag1=1;
              FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, circulation, 0, NULL);
            }

        }


		afficherMapEtTrain(ecran,tileset,table,LARGEUR_FENETRE/LARGEUR_TILE,HAUTEUR_FENETRE/HAUTEUR_TILE);
		afficherFeuRouge(ecran,tileset,table,LARGEUR_FENETRE/LARGEUR_TILE,HAUTEUR_FENETRE/HAUTEUR_TILE,car[0].etat);
		afficherVoitures(car,nbVoitures,imgCar,ecran);
		SDL_BlitSurface(imgTrain, NULL, ecran, &car[0].pos);		// le train (actualise son affichage)
		SDL_Flip(ecran);
	}
	FMOD_Sound_Release(circulation);
	FMOD_Sound_Release(train);
    FMOD_System_Close(system);//fermeture
    FMOD_System_Release(system);//libération espace
	SDL_FreeSurface(imgFond); /* On libère la surface */
	SDL_Quit();
	return EXIT_SUCCESS;
}
