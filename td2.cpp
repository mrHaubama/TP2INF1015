﻿/**
* TD2
* \file   td2.cpp
* \author Thomas Pronovost et Mikkelsen Bazelais
* \date   11 février 2025
* Créé le 1 février 2025
*/

#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include <span>

#include "cppitertools/range.hpp"

#include "bibliotheque_cours.hpp"
#include "verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
#include "debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).

using namespace std;
using namespace iter;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{

UInt8 lireUint8(istream& fichier)
{
	UInt8 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
UInt16 lireUint16(istream& fichier)
{
	UInt16 valeur = 0;
	fichier.read((char*)&valeur, sizeof(valeur));
	return valeur;
}
string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUint16(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}


//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.
span<Acteur*> spanListeActeur(ListeActeurs& listeActeurs) {
	return	span<Acteur*>(listeActeurs.elements, listeActeurs.nElements);
}

Acteur* chercherActeur(const ListeFilms& listeFilms, const string& nomActeur) {
	for (Film* filmPtr : listeFilms.spanListeFilms()) {
		for (Acteur* acteurExistantPtr : spanListeActeur(filmPtr->acteurs)) {
			if (acteurExistantPtr->nom == nomActeur) {
				return acteurExistantPtr;
			}
		}
	}
	return nullptr;
}


//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).
Acteur* lireActeur(istream& fichier, ListeFilms& listeFilms)
{
	Acteur acteur = {};
	acteur.nom            = lireString(fichier);
	acteur.anneeNaissance = lireUint16 (fichier);
	acteur.sexe           = lireUint8  (fichier);

	Acteur* acteurPtr = chercherActeur(listeFilms, acteur.nom);
	if (acteurPtr != nullptr) {
		// Libérer la liste de film du nouvelle acteur puisque l'on utilise un acteur déjà existant
		acteur.joueDans.libererTableauFilm();
		return acteurPtr;
	}

	cout << acteur.nom << endl;
	Acteur* nouvelleActeurPtr = new Acteur(acteur);
	return nouvelleActeurPtr; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
}

Film* lireFilm(istream& fichier, ListeFilms& listeFilms)
{
	Film film = {};
	film.titre       = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = lireUint16 (fichier);
	film.recette     = lireUint16 (fichier);

	film.acteurs.nElements = lireUint8 (fichier);  //NOTE: Vous avez le droit d'allouer d'un coup le tableau pour les acteurs, sans faire de réallocation comme pour ListeFilms.  Vous pouvez aussi copier-coller les fonctions d'allocation de ListeFilms ci-dessus dans des nouvelles fonctions et faire un remplacement de Film par Acteur, pour réutiliser cette réallocation.
	Film* nouveauFilm = new Film(film);
	nouveauFilm->acteurs.elements = new Acteur * [film.acteurs.nElements];
	nouveauFilm->acteurs.capacite = film.acteurs.nElements;
	for (int i = 0; i < film.acteurs.nElements; i++) {
		Acteur* acteurPtr = lireActeur(fichier, listeFilms); //TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
		acteurPtr->joueDans.ajouterFilm(nouveauFilm);
		nouveauFilm->acteurs.elements[i] = acteurPtr;
	}
	return nouveauFilm; //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	
	int nElements = lireUint16(fichier);

	//TODO: Créer une liste de films vide.
	ListeFilms listeFilms{};

	for (int i = 0; i < nElements; i++) {
		Film* filmPtr = lireFilm(fichier, listeFilms); //TODO: Ajouter le film à la liste.à
		listeFilms.ajouterFilm(filmPtr);
	}
	
	return listeFilms; //TODO: Retourner la liste de films.
}

void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ces acteurs (en utilisant la fonction afficherActeur ci-dessus).
void afficherListeActeur(Film& film) {
	for (Acteur* acteurPtr : spanListeActeur(film.acteurs)) {
		afficherActeur(*acteurPtr);
	}
}


//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.
void detruireFilm(ListeFilms& listeFilm, Film* filmADetruirePtr) {
	for (Film* filmPtr : listeFilm.spanListeFilms()) {

		for (Acteur* acteurPtr : spanListeActeur(filmPtr->acteurs)) {

			acteurPtr->joueDans.enleverFilm(filmADetruirePtr);
			if (acteurPtr->joueDans.obtenirNElements() == 0) {
				cout << "Suppression de l'acteur " << acteurPtr->nom << endl;

				//detruireListeFilms(acteurPtr->joueDans); // ATTENTION ERREUR??
				acteurPtr->joueDans.libererTableauFilm();
				//delete[] acteurPtr->joueDans.elements;
				delete acteurPtr;
				filmPtr->acteurs.nElements--;
			}
		}
	}

	listeFilm.enleverFilm(filmADetruirePtr);
	delete[] filmADetruirePtr->acteurs.elements;

	delete filmADetruirePtr;
}


//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
void detruireListeFilms(ListeFilms& listeFilms) {
	for (Film* filmToDeletePtr : listeFilms.spanListeFilms()) {
		detruireFilm(listeFilms, filmToDeletePtr);
	}
	listeFilms.libererTableauFilm();
}


void afficherListeFilms(const ListeFilms& listeFilms, int stop = 0) {
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35m########################################\033[0m\n";
	cout << ligneDeSeparation << endl;
	//TODO: Changer le for pour utiliser un span.
	if (stop == 0) {
		stop = listeFilms.obtenirNElements();
	}
	for (const Film* filmPtr : listeFilms.spanListeFilms().first(stop)) {
		//TODO: Afficher le film.
		cout << "  " << filmPtr->titre << ", " << endl;
		cout << "  " << filmPtr->realisateur << ", ";
		cout << "  " << filmPtr->anneeSortie << ", ";
		cout << "  " << filmPtr->recette << endl;
		Film film = *filmPtr;
		cout << "  liste d'acteurs dans le film:" << endl;
		afficherListeActeur(film);
		cout << ligneDeSeparation << endl;

	}
}


void afficherFilmographieActeur(const ListeFilms& listeFilms, const string& nomActeur)
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	Acteur* acteur = chercherActeur(listeFilms, nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		afficherListeFilms(acteur->joueDans);
}


int main()
{
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	//int* fuite = new int; //TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Fuite detectee" de "4 octets" affiché à la fin de l'exécution, qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms = creerListe("films.bin");
	
	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	afficherListeFilms(listeFilms, 1);
	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	afficherListeFilms(listeFilms);
	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	
	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	afficherFilmographieActeur(listeFilms, "Benedict Cumberbatch");
	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	detruireFilm(listeFilms, listeFilms.obtenirPremierFilmPtr());
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	afficherListeFilms(listeFilms);
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.
	
	//TODO: Détruire tout avant de terminer le programme.  La bibliothèque de verification_allocation devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	
	detruireListeFilms(listeFilms);
}
