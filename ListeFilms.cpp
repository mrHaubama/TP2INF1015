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

//TODO: Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; on veut uniquement ajouter le pointeur vers le film existant.  Cette fonction doit doubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y avait dans l'ancien, et éliminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ListeFilms::ajouterFilm(Film* filmPtr) {
	if (nElements_ == capacite_) {
		int nouvelleCapacite = 2 * capacite_;
		Film** nouvelleListeFilms = new Film* [nouvelleCapacite];

		for (int filmIndex : range(0, nElements_)) {
			nouvelleListeFilms[filmIndex] = elements_[filmIndex];
		}

		delete[] elements_;
		elements_ = nouvelleListeFilms;
		capacite_ = nouvelleCapacite;
	}

	int indexNouvelElement = nElements_;
	elements_[indexNouvelElement] = filmPtr;
	nElements_++;
}


//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
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

//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
void ListeFilms::detruireListeFilms() {
	for (Film* filmToDeletePtr : spanListeFilms()) {
		detruireFilm(filmToDeletePtr);
	}
	delete[] elements_;
}


int ListeFilms::getNElements() const {
	return nElements_;
}

void ListeFilms::afficherListeFilms() const{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = {};
	cout << ligneDeSeparation;
	//TODO: Changer le for pour utiliser un span.
	for (const Film* filmPtr : spanListeFilms()) {
		//TODO: Afficher le film.
		cout << "  " << filmPtr->titre;
		if (filmPtr != elements_[nElements_ - 1]) {
			cout << ", ";
		}
		cout << endl;
		cout << ligneDeSeparation;
	}
}