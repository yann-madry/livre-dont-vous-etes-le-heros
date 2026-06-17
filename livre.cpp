#include "livre.h"

Livre::Livre() {
    this->idPageDep = 1;
    this->pageErreur.setTitre("Page introuvable");
    this->pageErreur.setId(-1);
}

int Livre::getIdPageDepart() const{
    return this->idPageDep;
}

void Livre::setIdPageDepart(int id){
    this->idPageDep = id;
}

void Livre::ajouterPage(const Page &nouvellePage){
    this->livre.insert(nouvellePage.id(),nouvellePage);
}

bool Livre::contientPage(int id) const {
    if(this->livre.contains(id)){
        return true;
    }else return false;
}

Page& Livre::getPage(int id){
    if(Livre::contientPage(id)) return this->livre[id];
    return this->pageErreur;
}

void Livre::supprimerPage(int id){
    if(Livre::contientPage(id)) this->livre.remove(id);
}

QMap<int,Page>& Livre::getAllPages(){
    return this->livre;
}