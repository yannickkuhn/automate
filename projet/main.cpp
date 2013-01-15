#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#define SIZE 2

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

Uint32 affiche(Uint32 intervalle, void *parametre) {			// code à améliorer : il faudrait faire une fonction affichent ... pour toutes les voitures
	T_VOIT *voit;
	voit = parametre;
	voit->pos.x = voit->pos.x+voit->vitesse;
	if(voit->pos.x > 850)
		voit->pos.x = -50;
	return intervalle;
}

void setParamVoiture(T_VOIT *vehicule, int vitesse, int posH, int posV) {
	vehicule->vitesse = vitesse;
	vehicule->pos.x = posH;
	vehicule->pos.y = posV;
}

void affichageParamVoiture(T_VOIT vehicule) {
	printf("\nVoici les caractéristiques de celui ci : position : %d %d, vitesse : %d\n",vehicule.pos.x, vehicule.pos.y, vehicule.vitesse);
}

int main(int argc, char *argv[])
{
	// Variables
	SDL_Surface *ecran = NULL, *imgFond = NULL, *imgSapin = NULL;
	SDL_Rect pFond;
	SDL_Event event;
	T_VOIT car[SIZE];
	int continuer;

	// Variables du Timer
	SDL_TimerID idTimer, idTimer2;			// code à améliorer : utiliser 1 seul timer
	int periode = 10;

	// initialisation
	continuer = 1;
	pFond.x = 0;
	pFond.y = 0;
	setParamVoiture(&car[0],2,0,20);		// premiere voiture plus rapide
	setParamVoiture(&car[1],3,0,150);		// une deuxieme plus lente
	affichageParamVoiture(car[0]);
	affichageParamVoiture(car[1]);				// code à améliorer

	// SDL init
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	idTimer = SDL_AddTimer(periode,affiche,car);
	idTimer2 = SDL_AddTimer(periode,affiche,&car[1]);
	ecran = SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Premiere collision de vehicules ...", NULL);
	imgFond = SDL_LoadBMP("fond.bmp");
	imgSapin = SDL_LoadBMP("sapin.bmp");
	SDL_BlitSurface(imgFond, NULL, ecran, &pFond);
	SDL_BlitSurface(imgSapin, NULL, ecran, &car[0].pos);		// code à améliorer
	SDL_BlitSurface(imgSapin, NULL, ecran, &car[1].pos);
	SDL_Flip(ecran);

	while (continuer) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					continuer = 0;
					break;
			}
		}

		SDL_BlitSurface(imgFond, NULL, ecran, &pFond);
		SDL_BlitSurface(imgSapin, NULL, ecran, &car[0].pos);			// code à améliorer
		SDL_BlitSurface(imgSapin, NULL, ecran, &car[1].pos);
		SDL_Flip(ecran);
	}

	SDL_FreeSurface(imgFond); /* On libère la surface */
	SDL_Quit();
	return EXIT_SUCCESS;
}
