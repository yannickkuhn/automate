#include <stdio.h>
#define CMAX 50

// voici les sources pour l'automate cellulaire, écrits par Yannick et Julien

typedef struct cel {
	char type[CMAX];
	char etat[CMAX];
	int tps;
	int posH;
	int posV;
} T_CEL;

int main(void) {
	// déclaration des variables
	int touche;
	
	// programme : menu
	while(touche != 1) {
		printf("\n----------------------------------------------\n");
		printf("Menu de l'automate programmable:\n\t1) touche 1 : quitter\n\t2) touche 2 : sauvegarde\n\t3) touche 3 : charge\n\t4) touche 4 : creer/modifier cellules\n\t5) touche 5 : parametrer recadrage affichage\n\t6) touche 6 : calcul de l'automate jusq'a un temps donné sans affichage\n\t7) touche 7 : affichage de l'automate au temps donné\n\t8) touche 8 : calcul et affichage entre t0 et t1\n----------------------------------------------\nFaites votre choix ? ");
		scanf("%d",&touche);
		switch(touche) {
			case 1:
				printf("quitte le programme\n");
				break;
			case 2:
				printf("sauvegarde dans le fichier\n");
				break;
			case 3:
				printf("charger dans le fichier\n");
				break;
			case 4:
				printf("creer/modifier les cellules\n");
				break;
			case 5:
				printf("parametrer le recadrage de l'affichage\n");
				break;
			case 6:
				printf("calcul de l'automate jusqu'a un temps donné sans affichage\n");
				break;
			case 7:
				printf("affichage automate au temps donné\n");
				break;
			case 8:
				printf("calcul et affichage entre t0 et t1\n");
				break;
		};
		fflush(stdin);
	};
}
