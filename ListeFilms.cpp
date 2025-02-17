#include "structures.hpp"
#include "cppitertools/range.hpp"
#include "iostream"

using namespace std;
using namespace iter;

ListeFilms::ListeFilms():
	ListeFilms(1, 0, new Film* [1]) {
}

ListeFilms::ListeFilms(int capacite, int nElements, Film** elements):
	capacite_(capacite), nElements_(nElements), elements_(elements) {
}

void ListeFilms::agrandirListeFilm() {
	int nouvelleCapacite = 2 * capacite_;
	Film** nouvelleListeFilms = new Film * [nouvelleCapacite];

	for (int filmIndex : range(0, nElements_)) {
		nouvelleListeFilms[filmIndex] = elements_[filmIndex];
	}

	delete[] elements_;
	elements_ = nouvelleListeFilms;
	capacite_ = nouvelleCapacite;
}

//TODO: Une fonction pour ajouter un Film � une ListeFilms, le film existant d�j�; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau allou�, avec au minimum un �l�ment, dans le cas o� la capacit� est insuffisante pour ajouter l'�l�ment.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et �liminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ListeFilms::ajouterFilm(Film* filmPtr) {
	if (nElements_ == capacite_) {
		agrandirListeFilm();
	}

	int indexNouvelElement = nElements_;
	elements_[indexNouvelElement] = filmPtr;
	nElements_++;
}


//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en param�tre un pointeur vers le film � enlever.  L'ordre des films dans la liste n'a pas � �tre conserv�.
void ListeFilms::enleverFilm(Film* filmAEnleverPtr) {
	for (int filmIndex : range(0, nElements_)) {
		Film* filmPtr = elements_[filmIndex];
		if (filmAEnleverPtr == filmPtr) {
			Film* dernierFilmPtr = elements_[nElements_ - 1];
			elements_[filmIndex] = dernierFilmPtr;
			nElements_--;
		}
	}
}


span<Film*> ListeFilms::spanListeFilms() const {
	return span<Film*>(elements_, nElements_);
}


int ListeFilms::obtenirNElements() const {
	return nElements_;
}

void ListeFilms::libererTableauFilm() {
	delete[] elements_;
}

Film* ListeFilms::obtenirPremierFilmPtr() const {
	return elements_[0];
}