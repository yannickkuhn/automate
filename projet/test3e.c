#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#define SIZE 3

#define LARGEUR_TILE 128
#define HAUTEUR_TILE 96
#define LARGEUR_FENETRE 896
#define HAUTEUR_FENETRE 576

// Ce code du 13/01/2012 [3] permet d'afficher deux voitures et de les faire bouger ...

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

void Afficher(SDL_Surface* screen,SDL_Surface* tileset,char** table,int nombre_blocs_largeur,int nombre_blocs_hauteur)
{

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
void Afficher1(SDL_Surface* screen,SDL_Surface* tileset,char** table,int nombre_blocs_largeur,int nombre_blocs_hauteur,int arret)
{

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


Uint32 affiche(Uint32 intervalle, void *parametre) {			// code à améliorer : il faudrait faire une fonction affichent ... pour toutes les voitures
	T_VOIT *voit;
	voit = parametre;
	/*if(voit->etat==0)
	{
    voit->pos.x = voit->pos.x+voit->vitesse;
	}
    else
    {
       if(voit->pos.x <200)
       {
        voit->pos.x = voit->pos.x+voit->vitesse-1;
       }

      else
         {
            if((voit->pos.x >=300)&&(voit->pos.x <450))
            {
             voit->pos.x = voit->pos.x+voit->vitesse+5;
            }
            else
            {
             if(voit->pos.x >=450)
         voit->pos.x = voit->pos.x+voit->vitesse;
            }
         }


    }*/
    if(voit->etat == 0)
        voit->pos.x = voit->pos.x+voit->vitesse;


	if(voit->pos.x > 850)
		voit->pos.x = -50;
	return intervalle;
}
Uint32 affiche1(Uint32 intervalle, void *parametre) {			// code à améliorer : il faudrait faire une fonction affichent ... pour toutes les voitures
	T_VOIT *voit;
	voit = parametre;
    voit->pos.y = voit->pos.y+voit->vitesse;
	if(voit->pos.y > 800)
		voit->pos.y = -300;
	return intervalle;

}


void setParamVoiture(T_VOIT *vehicule, int vitesse, int posH, int posV,int arret) {
	vehicule->vitesse = vitesse;
	vehicule->pos.x = posH;
	vehicule->pos.y = posV;
	vehicule->etat = arret;
}

void load(T_VOIT *vehicule,int arret) {

	vehicule->etat = arret;
}

void affichageParamVoiture(T_VOIT vehicule) {
	printf("\nVoici les caractéristiques de celui ci : position : %d %d, vitesse : %d\n",vehicule.pos.x, vehicule.pos.y, vehicule.vitesse);
}

int test(T_VOIT *vehicule)
{
 if((vehicule->pos.y>-300)&&(vehicule->pos.y<500))
 {
     return 1;
 }
 else
 {
      return 0;
 }
}

int ControleArretVoitures(T_VOIT *voit, int nbVoit, int arret) {
    int i;
    if(arret == 1) {            // si train present
        for(i=0;i<nbVoit;i++) {
            if(voit[i].pos.x > 150 && voit[i].pos.x < 200) {
                voit[i].etat = 1;
            }
        }
    }
    if(arret == 0) {
        for(i=0;i<nbVoit;i++) {
            voit[i].etat = 0;
        }
    }
}


int main(int argc, char *argv[])
{
	// Variables
	SDL_Surface *ecran = NULL, *imgFond = NULL, *imgSapin = NULL, *tileset = NULL, *imgtrain = NULL;
	SDL_Rect pFond;
	SDL_Event event;
	T_VOIT car[SIZE];
	int continuer;
	int res;
	int arret=0;
	// Variables du Timer
	SDL_TimerID idTimer, idTimer2,idTimer3;			// code à améliorer : utiliser 1 seul timer
	int periode = 10;

	// initialisation
	continuer = 1;
	setParamVoiture(&car[2],1,358,0,arret);
	setParamVoiture(&car[0],2,0,230,arret);		// premiere voiture plus rapide
    setParamVoiture(&car[1],3,0,320,arret);
	affichageParamVoiture(car[0]);
	affichageParamVoiture(car[1]);
	affichageParamVoiture(car[2]);			// code à amélioreR
    //SDL_RemoveTimer(idTimer)
	// chargement de la map
	tileset = SDL_LoadBMP("route.bmp");

	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	idTimer = SDL_AddTimer(periode,affiche,car);
	idTimer2 = SDL_AddTimer(periode,affiche,&car[1]);
	idTimer3 = SDL_AddTimer(periode,affiche1,&car[2]);
	ecran = SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Premiere collision de vehicules ...", NULL);
	imgSapin = SDL_LoadBMP("voiture.bmp");
	imgtrain = SDL_LoadBMP("train.bmp");
	Afficher(ecran,tileset,table,LARGEUR_FENETRE/LARGEUR_TILE,HAUTEUR_FENETRE/HAUTEUR_TILE);
	SDL_BlitSurface(imgSapin, NULL, ecran, &car[0].pos);		// code à améliorer
	SDL_BlitSurface(imgSapin, NULL, ecran, &car[1].pos);
	SDL_BlitSurface(imgtrain, NULL, ecran, &car[2].pos);
	SDL_Flip(ecran);

	while (continuer) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					continuer = 0;
					break;
			}
		}
        res=test(&car[2]);
        if(res==1)
        {
        arret=1;
        }
        else
        {
        arret=0;
        }
        /*load(&car[0],arret);
        load(&car[1],arret);*/
		Afficher(ecran,tileset,table,LARGEUR_FENETRE/LARGEUR_TILE,HAUTEUR_FENETRE/HAUTEUR_TILE);
		Afficher1(ecran,tileset,table,LARGEUR_FENETRE/LARGEUR_TILE,HAUTEUR_FENETRE/HAUTEUR_TILE,arret);
        ControleArretVoitures(car,2,arret);
        SDL_BlitSurface(imgSapin, NULL, ecran, &car[0].pos);
        SDL_BlitSurface(imgSapin, NULL, ecran, &car[1].pos);
		SDL_BlitSurface(imgtrain, NULL, ecran, &car[2].pos);
		SDL_Flip(ecran);
	}

	SDL_FreeSurface(imgFond); /* On libère la surface */
	SDL_Quit();
	return EXIT_SUCCESS;
}

