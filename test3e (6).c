#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#define NBCARMAX 10+1		// le train compte pour 1
#include <fmod.h>
#define LARGEUR_TILE 128
#define HAUTEUR_TILE 96
#define LARGEUR_FENETRE 896
#define HAUTEUR_FENETRE 576
#define POSITION_TOUCHE_MENU_X 50
#define LARGEUR_TOUCHE_MENU 54
#define HAUTEUR_TOUCHE_MENU 54
#define POSITION_TOUCHE_MENU_Y 37
#include <SDL/SDL_ttf.h>

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
// - gestion de la musique (julien gère cette partie)
//   ____________


typedef struct voiture {
	char etat;
	char enattente;
	SDL_Rect pos;
	int vitesse;
} T_VOIT;

typedef struct bouton {
	char etat;
	SDL_Rect pos;
} T_BOUTON;

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
	  if(arret==0) {
		Rect_source.x = ('>'-'0')*LARGEUR_TILE;
	  }
	  else {
		Rect_source.x = ('6'-'0')*LARGEUR_TILE;
	  }
	SDL_BlitSurface(tileset,&Rect_source,screen,&Rect_dest);
}

int calculPosYDeLaVoie(int voie) {
	return 230+voie*90;
}

int calculVoieDeLaPosY(int posy) {
	return (posy-230)/90;
}

void afficherVoitures(T_VOIT *voit, int nbVoitures, SDL_Surface *img, SDL_Surface *ecran) {
	int i,v;
	int posy, nbVoitVoie[2]={0};
	int nbVoiesMax=2;
	int remiseVehiculeAPosNulle = 0;

	// gestion des collisions à rajouter ultérieurement !
	for(v=0;v<nbVoiesMax;v++) {
		posy = calculPosYDeLaVoie(v);	// calcul de la position y pour chaque voies

		for(i=1;i<nbVoitures;i++) {
			if(voit[i].pos.y == posy && voit[i].etat == 1) {		// si la voiture est sur la voie 0 et qu'elle est arreter
				nbVoitVoie[v]++;		// on incrémente le nombre de voitures de la voie 0
			}
		}
	}

	//printf("pos y de la voie 0 : %d nombre de voitures arretées sur la voie 0 : %d \n",posy,nbVoitVoie0);

	// gestion du feu stop pour les voitures (lorsque le train passe)
	int idTrain = 0;
	for(v=0;v<nbVoiesMax;v++) {

		posy = calculPosYDeLaVoie(v);
		remiseVehiculeAPosNulle = 0;

		for(i=1;i<nbVoitures;i++) {
			if(voit[i].pos.y == posy) {
				if(voit[0].etat == 1) {		// si le train impose le feu rouge (circule sur la voie)
					if(180-nbVoitVoie[v]*150 > 0) {
						if(voit[i].pos.x >= (180-nbVoitVoie[v]*150) && voit[i].pos.x < (200-nbVoitVoie[v]*150) ) {		// les voitures, lorsqu'elles arrivent entre la pos 180|200, s'arretent
							voit[i].etat = 1;
						}
					}
					else {
						// etat d'attente de la voiture (elle est a gauche de l'ecran visible)
						if(voit[i].pos.x > 200) {
							voit[i].enattente = 1;
						}
					}
				}
				else {						// si le train est reparti, les voitures peuvent repartir ...
					if(voit[i].etat == 1) {
						voit[i].etat = 0;
					}
					else {	// les vehicules qui sont en dehors de l'ecran doivent revenir, [!] on part toujours de l'esprit que la première voiture est celle qui est toujours celle la plus en avance par rapport aux autres
						if(voit[i].pos.x > 850) {
							if(PresenceVoituresEnPosNulle(voit,nbVoitures,posy) == 0 && remiseVehiculeAPosNulle == 0) {
								voit[i].enattente = 0;
								remiseVehiculeAPosNulle++;
							}
						}
					}
				}
			}
			// reactualisation de l'affichage des voitures (dans l'ecran)
			if(voit[i].pos.x >=0 && voit[i].pos.x <= 900)
				SDL_BlitSurface(img, NULL, ecran, &voit[i].pos);
		}

	}
}

Uint32 actualisePosVoitures(Uint32 intervalle, void *parametre) {
	T_VOIT *voit;
	int i;
	voit = parametre;
	for(i=1;i<NBCARMAX;i++) {
		if(voit[i].etat == 0) {		// si la voiture peut avancée
			voit[i].pos.x = voit[i].pos.x+voit[i].vitesse;
			if(voit[i].pos.x > 850 && voit[i].enattente == 0)
				voit[i].pos.x = 0;
		}
	}
	return intervalle;
}

Uint32 actualisePosTrain(Uint32 intervalle, void *parametre) {
	T_VOIT *voit;
	voit = parametre;
    voit->pos.y = voit->pos.y+voit->vitesse;
	if(voit->pos.y > 1500)
		voit->pos.y = 0;
	return intervalle;
}

void setParamVoiture(T_VOIT *vehicule, int vitesse, int posH, int posV) {
	vehicule->vitesse = vitesse;
	vehicule->pos.x = posH;
	vehicule->pos.y = posV;
	vehicule->etat = 0;
	vehicule->enattente = 0;
}

void affichageParamVoiture(T_VOIT vehicule) {
	printf("\nVoici les caractéristiques de celui ci : position : %d %d, vitesse : %d\n",vehicule.pos.x, vehicule.pos.y, vehicule.vitesse);
}

int testDuPassageDuTrain(T_VOIT *vehicule) {
	//if((vehicule->pos.y>-300)&&(vehicule->pos.y<500))
	if((vehicule->pos.y>1000)||(vehicule->pos.y>-300)&&(vehicule->pos.y<500))
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
	setParamVoiture(&voit, 1, 0, calculPosYDeLaVoie(voie));
	return voit;
}

void setParambouton(T_BOUTON *bouton,int etat, int pox, int poy)
{
	bouton->pos.x = pox;
	bouton->pos.y = poy;
	bouton->etat = 0;
}



/* CODE DU PROGRAMME PRINCIPAL */

int main(int argc, char *argv[])
{
	// Variables
	SDL_Surface *ecran = NULL,*texte=NULL,*polytech=NULL, *imgFond = NULL, *imgCar = NULL, *tileset = NULL, *imgTrain = NULL, *menutrain=NULL,*bouton1=NULL,*bouton0=NULL,*bouton2=NULL;
	SDL_Rect position_texte_menu;
	SDL_Event event;//variable event
	T_VOIT car[NBCARMAX], cartmp;
	T_BOUTON bout[8];
	int continuer;
	int menucontinuer;
	int nbVoitures;
	int flag;
	int flag1;
	flag=0;
	flag1=0;
	int flag2;
	flag2=0;
	int transparence;
	int i;
	int j;
	int u;

    int tabbouton[8];
    int positionsourisx;
    int positionsourisy;
	// Variables du Timer
	SDL_TimerID idTimerTrain, idTimerCar;
	int periode;
	int tmp;
    tmp=0;
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
    FMOD_SOUND *circulation = NULL,*train = NULL,*menu=NULL,*defilement=NULL,*rebond=NULL;
    FMOD_System_CreateSound(system, "circulation.wav", FMOD_CREATESAMPLE, 0, &circulation);
    FMOD_System_CreateSound(system, "train.wav", FMOD_CREATESAMPLE, 0, &train);
    FMOD_System_CreateSound(system, "menu.mid", FMOD_CREATESAMPLE, 0, &menu);
    FMOD_System_CreateSound(system, "defilement.wav", FMOD_CREATESAMPLE, 0, &defilement);
    FMOD_System_CreateSound(system, "rebon.wav", FMOD_CREATESAMPLE, 0, &rebond);
	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	idTimerTrain = SDL_AddTimer(periode,actualisePosTrain,&car[0]);
	idTimerCar = SDL_AddTimer(periode,actualisePosVoitures,car);
	ecran = SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Premiere collision de vehicules ...", NULL);
	imgCar = SDL_LoadBMP("voiture.bmp");
	imgTrain = SDL_LoadBMP("train.bmp");
	menutrain = SDL_LoadBMP("menutrain.bmp");
	bouton1 = SDL_LoadBMP("boutonappuyer.bmp");
	bouton2 = SDL_LoadBMP("boutonselectionner.bmp");
    bouton0 = SDL_LoadBMP("bouton.bmp");
    polytech = SDL_LoadBMP("polytech.bmp");
    SDL_SetColorKey(bouton0, SDL_SRCCOLORKEY, SDL_MapRGB(bouton0->format, 255, 255, 255));
    SDL_SetColorKey(bouton1, SDL_SRCCOLORKEY, SDL_MapRGB(bouton0->format, 255, 255, 255));
//



    TTF_Init();
    TTF_Font *police = NULL;
    SDL_Color couleurNoire = {255,255,255};
    police = TTF_OpenFont("airwaves.ttf", 50);
    position_texte_menu.x = 124;


     for(i=0;i<8;i++)
       {
        setParambouton(&bout[i],0, 50,37+64*i);
       }
       for (i =0 ; i <=255 ; i++)
{
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, i, i, i));
    SDL_SetAlpha(polytech, SDL_SRCALPHA, i);
    SDL_BlitSurface(polytech, NULL, ecran,NULL);
    SDL_Flip(ecran);
    SDL_Delay(5);
}

SDL_Delay(500);

    for (i =255 ; i >=0 ; i--)
{
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, i, i, i));
    SDL_SetAlpha(polytech, SDL_SRCALPHA, i);
    SDL_BlitSurface(polytech, NULL, ecran,NULL);
    SDL_Flip(ecran);
    SDL_Delay(5);
}


 //FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, menu, 0, NULL);
       for(i=0;i<8;i++)
       {
           if(i==0)
           {
             for(j=0;j<485;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           SDL_BlitSurface(bouton0, NULL, ecran, &bout[i].pos);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }
           if(i==1)
           {
             for(j=0;j<421;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }
                if(i==2)
           {
             for(j=0;j<357;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }
           if(i==3)
           {
             for(j=0;j<295;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }
           if(i==4)
           {
             for(j=0;j<233;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }
           if(i==5)
           {
             for(j=0;j<171;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }
            if(i==6)
           {
             for(j=0;j<109;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }
           if(i==7)
           {
             for(j=0;j<47;j=j+5)
           {
           setParambouton(&bout[i],0, 50,j);
           SDL_BlitSurface(menutrain, NULL, ecran, NULL);
           for(u=0;u<=i;u++)
           {
            SDL_BlitSurface(bouton0, NULL, ecran, &bout[u].pos);
           }
           SDL_Flip(ecran);
           }
           FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, rebond, 0, NULL);
           }

       }

	while (menucontinuer)
{
    SDL_BlitSurface(menutrain, NULL, ecran, NULL);
    SDL_EnableKeyRepeat(10, 10);//repetition
    SDL_PollEvent(&event);
    switch(event.type)
    {
        case SDL_QUIT:
            menucontinuer = 0;
            break;


    }
    for(i=0; i< 8;i++)
                    {

                if ((event.motion.x>POSITION_TOUCHE_MENU_X)&&(event.motion.x<(POSITION_TOUCHE_MENU_X+LARGEUR_TOUCHE_MENU))&&(event.motion.y>(POSITION_TOUCHE_MENU_Y+i*62))&&(event.motion.y<((POSITION_TOUCHE_MENU_Y+i*62)+HAUTEUR_TOUCHE_MENU)))
                    {
                      if (event.button.button == SDL_BUTTON_RIGHT)
                      {
                       bout[i].etat=2;
                      }
                      bout[i].etat=1;
                    }
                  else
                  {
                     bout[i].etat=0;
                  }


                    }



    for(i=0;i<8;i++)
 {
     if(bout[i].etat==0)
     {
         if(bout[tmp].etat==0)
         {
           flag2=0;
         }
      SDL_BlitSurface(bouton0, NULL, ecran, &bout[7-i].pos);
     }
     else
     {

          if(flag2==0)
         {
             tmp=i;
             flag2=1;
             FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, defilement, 0, NULL);
         }
         if(bout[0].etat==1)
         {
         position_texte_menu.y = 42;
         texte = TTF_RenderText_Blended(police, "1 :QUITTER", couleurNoire);
         SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
         }
         if(bout[1].etat==1)
         {

                position_texte_menu.y = 104;
                texte = TTF_RenderText_Blended(police, "2 :LANCER", couleurNoire);
                SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
        }


        if(bout[1].etat==2)
         {
          menucontinuer = 0;
          break;
         }



         if(bout[2].etat==1)
         {
         position_texte_menu.y = 166;
         texte = TTF_RenderText_Blended(police, "3 :TEMPS", couleurNoire);
         SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
         }
         if(bout[3].etat==1)
         {
         position_texte_menu.y = 228;
         texte = TTF_RenderText_Blended(police, "4 : MUSIQUE", couleurNoire);
         SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
         }
         if(bout[4].etat==1)
         {
         position_texte_menu.y = 290;
         texte = TTF_RenderText_Blended(police, "5 : FONCTIONS", couleurNoire);
         SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
         }
         if(bout[5].etat==1)
         {
         position_texte_menu.y = 352;
         texte = TTF_RenderText_Blended(police, "6 :INITIALISER", couleurNoire);
         SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
         }
         if(bout[6].etat==1)
         {
         position_texte_menu.y = 414;
         texte = TTF_RenderText_Blended(police, "7 :REGLAGES", couleurNoire);
         SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
         }
         if(bout[7].etat==1)
         {
         position_texte_menu.y = 476;
         texte = TTF_RenderText_Blended(police, "8 :PARAMETRES", couleurNoire);
         SDL_BlitSurface(texte, NULL, ecran, &position_texte_menu); /* Blit du texte */
         }
         SDL_BlitSurface(bouton2, NULL, ecran, &bout[7-i].pos);
         }



 }

	       SDL_Flip(ecran);
}
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
    FMOD_System_Release(system);//li
	SDL_FreeSurface(imgFond); /* On libère la surface */
    TTF_CloseFont(police);
    TTF_Quit();
 	SDL_Quit();
	return EXIT_SUCCESS;
}
