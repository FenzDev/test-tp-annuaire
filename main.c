#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


typedef struct {
    char Num[15];
    struct NumNode* Next;
} NumNode;

typedef NumNode* NumPtr;

typedef struct {
    char Nom[50];
    char Prenom[50];
    char Relation[25];
    NumPtr Nums;
    struct contact* Next;
} contact;

typedef contact* Annuaire;

NumPtr createNumsList(char **nums, int size){
    NumPtr L = NULL;
    for (int i = size-1; i >= 0; i--) {
        NumPtr n = (NumPtr)malloc(sizeof(NumNode));
        strcpy(n->Num, nums[i]);
        n->Next = L;
        L = n;
    }
    return L;
}

contact* searchContact(Annuaire A, char nom[], char prenom[]) {
    contact* c = A;
    while (c != NULL) {
        if (strcmp(c->Nom,nom) == 0 && strcmp(c->Prenom,prenom) == 0) {
           return c;
        }
        c = c->Next;
    }
    return NULL;
}

void addContact(Annuaire* A, char nom[], char prenom[], char relation[], NumPtr nums) {
    contact* c = (contact*)malloc(sizeof(contact));
    if (c != NULL) {
        strcpy(c->Prenom, prenom);
        strcpy(c->Nom, nom);
        strcpy(c->Relation, relation);
        c->Nums = nums;
        c->Next = *A;
        *A = c;
    }
}

void deleteContact(Annuaire* A, char nom[], char prenom[]) {
    contact *c = *A, *cPrv = NULL;
    while (c != NULL) {
        if (strcmp(c->Nom,nom) == 0 && strcmp(c->Prenom,prenom) == 0) {
            if (c == *A)
                *A = c->Next;
            else
                cPrv->Next = c->Next;

            NumPtr n = c->Nums;
            while (n != NULL){
                NumPtr nTmp = n;
                n = n->Next;
                free(n);
            }
            free(c);
            return;
        }
        cPrv = c;
        c = c->Next;
    }
}

void modifyContact(Annuaire* A, char nom[], char prenom[], char novNom[], char novPrenom[], char novRel[]) {
    contact *c = *A;
    while (c != NULL) {
        if (strcmp(c->Nom,nom) == 0 && strcmp(c->Prenom,prenom) == 0) {
            strcpy(c->Nom, novNom);
            strcpy(c->Prenom, novPrenom);
            strcpy(c->Relation, novRel);
            return;
        }
        c = c->Next;
    }
}

void saveAnnuaire(Annuaire A, char* fname) {
    FILE* f = fopen(fname, "wb");
    if (f != NULL) {
        contact* c = A;
        while (c != NULL) {
            fwrite(c->Nom, sizeof(c->Nom), 1, f);
            fwrite(c->Prenom, sizeof(c->Prenom), 1, f);
            fwrite(c->Relation, sizeof(c->Relation), 1, f);
            NumPtr n = c->Nums;
            while (n != NULL ){
                fwrite(n->Num, sizeof(n->Num), 1, f);
                n = n->Next;
            }
            char nullStr[sizeof(n->Num)] = "";
            fwrite(nullStr, sizeof(nullStr), 1, f);

            if (c->Next != NULL){
                fwrite(">", sizeof(char), 1, f); // bch yindiquer bli kayn contact morah
            } else {
                fwrite("X", sizeof(char), 1, f); // bch yindiquer bli fmak y7bs annuaire
            }

            c = c->Next;
        }

        fclose(f);
    }
}

int loadAnnuaire(Annuaire* A, char* fname) {
    while ((*A) != NULL) {
        deleteContact(A, (*A)->Nom, (*A)->Prenom);
    }

    FILE* f = fopen(fname, "rb");
    if (f != NULL) {
        char whatsNext;
        do {
            contact c;
            if (!fread(c.Nom, sizeof(c.Nom), 1, f )) return 0;
            if (!fread(c.Prenom, sizeof(c.Prenom), 1, f)) return 0;
            if (!fread(c.Relation, sizeof(c.Relation), 1, f)) return 0;
            c.Nums = NULL;

            NumPtr tail = NULL;
            char numBuff[sizeof(c.Nums->Num)];
            do {
                if (!fread(numBuff, sizeof(numBuff), 1, f)) return 0;
                if (numBuff[0] == '\0') break;

                NumPtr n = (NumPtr)malloc(sizeof(NumNode));

                if (n == NULL) return 0;

                n->Next = NULL;

                strcpy(n->Num, numBuff);
                if (c.Nums == NULL) {
                    c.Nums = n;
                    tail = n;
                } else {
                    tail->Next = n;
                    tail = tail->Next;
                }
            } while (!feof(f));

            addContact(A, c.Nom, c.Prenom, c.Relation, c.Nums);

            fread(&whatsNext, sizeof(char), 1, f);

        } while (whatsNext == '>');
        fclose(f);
        return 1;
    }
    *A = NULL;
    return 0;
}

Annuaire subContacts(Annuaire A, char criteria[]) {
    Annuaire subA = NULL;
    contact* subATail = NULL;
    contact *c = A;

    while (c != NULL) {
        if (strcmp(criteria, c->Relation) == 0) {
            contact* newC = (contact*)malloc(sizeof(contact));
            if (subA == NULL) {
                subA = newC;
                subATail = subA;
            } else {
                subATail->Next = newC;
                subATail = newC;
            }
            subATail->Next = NULL;
            strcpy(subATail->Nom, c->Nom);
            strcpy(subATail->Prenom, c->Prenom);
            strcpy(subATail->Relation, c->Relation);

            NumPtr n = c->Nums;
            NumPtr nums = NULL, numsTail = NULL;
            while (n != NULL) {
                NumPtr newN = (NumPtr)malloc(sizeof(NumNode));
                newN->Next = NULL;
                strcpy(newN->Num, n->Num);
                if (nums == NULL) {
                    nums = newN;
                    numsTail = newN;
                } else {
                    numsTail->Next = newN;
                    numsTail = newN;
                }

                n = n->Next;
            }
            subATail->Nums = nums;

        }
        c = c->Next;
    }
    return subA;
}

void mergeContacts(Annuaire* A) {

    contact *c = *A;
    while (c != NULL) {
        contact *dPrv = c;
        contact *d = c->Next;
        while (d != NULL) {
            if ((strcmp(c->Nom, d->Nom) == 0) &&
                (strcmp(c->Prenom, d->Prenom) == 0) &&
                (strcmp(c->Relation, d->Relation) == 0))
            {
                dPrv->Next = d->Next;
                NumPtr cTail = c->Nums;
                while (cTail->Next != NULL) cTail = cTail ->Next;
                cTail->Next = d->Nums;

                NumPtr dTmp = d;
                d = d->Next;
                free(dTmp);
            } else {
                dPrv = d;
                d = d->Next;
            }

        }
        c = c->Next;
    }

}

void printContact(contact c) {
    printf("Nom:       %s\n", c.Nom);
    printf("Prenom:    %s\n", c.Prenom);
    printf("Relation:  %s\n", c.Relation);
    printf("Numeros: ");
    NumPtr n = c.Nums;
    while (n != NULL) {
        printf("- %s\n", n->Num);

        if (n->Next != NULL) printf("         ");

        n = n->Next;
    }
    printf("---\n");
}

void printAnnuaire(Annuaire A) {
    contact * c = A;
    printf("---\n");
    if (A == NULL) {
        printf("Annuaire est vide\n---\n");
    }
    while (c != NULL) {
        printContact(*c);
        c = c->Next;
    }
}



// Menu System
#pragma region Menu

Annuaire currentA = NULL;

contact readNomPrenom(){
    contact c;

    printf("Donnez le nom:  ");
    fflush(stdin);
    fgets(c.Nom, sizeof(c.Nom), stdin);
    c.Nom[strlen(c.Nom)-1] = '\0';

    printf("Donnez le prenom:  ");
    fflush(stdin);
    fgets(c.Prenom, sizeof(c.Prenom), stdin);
    c.Nom[strlen(c.Prenom)-1] = '\0';

    return c;
}

contact readNomPrenomRelation() {
    contact c = readNomPrenom();

    printf("Donnez le lien/relation:  ");
    fflush(stdin);
    fgets(c.Relation, sizeof(c.Relation), stdin);
    c.Nom[strlen(c.Relation)-1] = '\0';


    return c;
}

void choiceRechercher() {
    contact c = readNomPrenom();

    contact* foundC = searchContact(currentA, c.Nom, c.Prenom);

    if (foundC == NULL) {
        printf("\nAucune contacte avec ces information.\n");
    } else {
        printf("---\n");
        printContact(c);
    }

}

void choiceAjouter( ) {

    contact c = readNomPrenomRelation();

    c.Nums = NULL; int otherNumber = 0;
    do {
        NumPtr n = (NumPtr)malloc(sizeof(NumNode));

        printf("Donner un numero:  ");
        fflush(stdin);
        fgets(n->Num, sizeof(n->Num), stdin);

        n->Next = c.Nums;
        c.Nums = n;

        char ch;

        do {
            printf("Pour ajouter un autre entrez 'y'/'N':  ");
            fflush(stdin);
            ch = fgetc(stdin);
        } while (ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N');

        if (ch == 'y' || ch == 'Y') {
            otherNumber = 1;
        } else {
            otherNumber = 0;
        }
    } while (otherNumber);

    addContact(&currentA, c.Nom, c.Prenom, c.Relation, c.Nums);
    printf("\nContact est ajoute !\n");

}

void choiceSupprimer( ) {
    contact c = readNomPrenom();
    contact* foundC = searchContact(currentA, c.Nom, c.Prenom);

    if (foundC == NULL) {
        printf("\nAucune contacte avec ces information.\n");
    } else {
        deleteContact(&currentA, c.Nom, c.Prenom);
        printf("\nContact est supprime !\n");
    }
}

void choiceModifier( ) {
    contact c = readNomPrenom();

    contact newC;

    printf("Donnez un noveau nom:    ");
    fflush(stdin);
    fgets(newC.Nom, sizeof(c.Nom), stdin);

    printf("Donnez un noveau prenom: ");
    fflush(stdin);
    fgets(newC.Prenom, sizeof(c.Prenom), stdin);

    printf("Donnez un noveau lien/relation : ");
    fflush(stdin);
    fgets(newC.Relation, sizeof(c.Relation), stdin);

    modifyContact(&currentA, c.Nom, c.Prenom, newC.Nom, newC.Prenom, newC.Relation );

    printf("Contact est modifie !\n");
}

void choiceSauvgarder() {
    char fname[100];
    printf("Donnez le nom de fichier pour sauvegarder : \n");
    fflush(stdin);
    fgets(fname, sizeof(fname), stdin);
    int len = strlen(fname);
    if (len>0) fname[len-1] = '\0'; // remove trailing \n

    saveAnnuaire(currentA, fname);

    printf("Contact est sauvgarde !\n");
}

void choiceCharger() {
    if (currentA != NULL) {
        char save;
        do {
            printf("Voulez-vous enregistrer l'annuaire actuelle avant cela ? (y/N)\n");
            fflush(stdin);
            scanf("%c", &save);
        } while (save != 'y' && save != 'Y' && save != 'n' && save != 'N');
        if (save == 'y' || save == 'Y')
            choiceSauvgarder();
    }

    char fname[100];
    printf("Donnez le nom de fichier pour charger : \n");
    fflush(stdin);
    fgets(fname, sizeof(fname), stdin);
    int len = strlen(fname);
    if (len>0) fname[len-1] = '\0'; // remove trailing \n

    int succeed = loadAnnuaire(&currentA, fname);

    if (succeed)
        printf("Contact est chargee !\n");
    else
        printf("Contact est ne pas chargee !\n");
}

void choiceFusionner() {
    mergeContacts(&currentA);

    printf("Les contacts doubles sont fusionee !\n");
}

void choiceCreerSousCarnet() {

    char crit[25];

    printf("Donnez un lien/relation pour creer un sous carnet: \n");
    fflush(stdin);
    fgets(crit, sizeof(crit), stdin);

    Annuaire subA = subContacts(currentA, crit);

    if (subA == NULL) {
        printf("Carnet n'est pas cree ! (aucune correspondance) \n");
    } else {
        printf("Le carnet est cree !\n");

        printAnnuaire(subA);
    }

    char save;
    do {
        printf("Voulez-vous enregistrer l'sous-annuaire ? (y/N)\n");
        fflush(stdin);
        scanf("%c", &save);
    } while (save != 'y' && save != 'Y' && save != 'n' && save != 'N');
    if (save == 'y' || save == 'Y'){
        Annuaire baseA = currentA;
        currentA = subA;
        choiceSauvgarder();
        currentA = baseA;
    }

    while (subA != NULL){
        deleteContact(&subA, subA->Nom, subA->Prenom);
    }
}

void choiceAfficherAnnuaire() {
    printAnnuaire(currentA);

    printf("Appuyez sur 'Entrez' pour revenir au menu\n");
    fflush(stdin);
    getchar();
}

void printMenu() {
    printf("\n");

    printf(
"1.  Rechercher un contact par nom et prenom.\n"
"2.  Ajouter un nouveau contact.\n"
"3.  Supprimer un contact existant.\n"
"4.  Modifier les informations d'un contact.\n"
"5.  Sauvegarder la liste de contacts dans un fichier: Enregistrer tous les contacts sur\n"
"    l'ordinateur pour pouvoir les retrouver plus tard.\n"
"6.  Charger la liste de contacts depuis un fichier: Ouvrir le fichier pour recuperer tous\n"
"    les contacts et les utiliser dans le programme.\n"
"7.  Fusionner les contacts en double: Si une personne appara�t plusieurs fois, regrouper\n"
"    toutes ses informations et tous ses numeros dans un seul contact.\n"
"8.  Creer un sous-carnet de contacts selon un critere: Par exemple, creer un nouveau\n"
"    carnet contenant uniquement les contacts de la famille ou uniquement les amis.\n"
"9.  Afficher tout les contacts de l'Annuaire.\n"
"0.  Quitter le programe.\n"
        );
    printf("\n");
}

int applyChoice(int choice){
    char sure, save;
    switch (choice) {
        case 1: choiceRechercher();
            break;
        case 2: choiceAjouter();
            break;
        case 3: choiceSupprimer();
            break;
        case 4: choiceModifier();
            break;
        case 5: choiceSauvgarder();
            break;
        case 6: choiceCharger();
            break;
        case 7: choiceFusionner();
            break;
        case 8: choiceCreerSousCarnet();
            break;
        case 9: choiceAfficherAnnuaire();
            break;
        case 0:
            printf("Etes-vous sur de vouloir quitter (y/N)? \n");
            fflush(stdin);
            scanf("%c", &sure);
            if (sure != 'y' && sure != 'Y') return 0;
            do {
                printf("Voulez-vous enregistrer l'annuaire avant cela ? (y/N)\n");
                fflush(stdin);
                scanf("%c", &save);
            } while (save != 'y' && save != 'Y' && save != 'n' && save != 'N');
            if (save == 'y' || save == 'Y')
                choiceSauvgarder();
            return 1;
    }
    return 0;
}

void menu(){

    int choice;
    int exit = 0;
    while (!exit) {
        printMenu();
        printf("> "); scanf("%d", &choice);
        printf("\n");
        exit = applyChoice(choice);
        fflush(stdin);
    }

}

#pragma endregion



int main()
{
    // Pour mode menu decommenter:
    // menu();
    // return 0;

    Annuaire A = NULL;

    printf("Init:\n");
    printAnnuaire(A);
    getchar();

    // --------------

    char *n[] = {"+21325123123"};
    addContact(&A, "Feniche", "Moncef", "Me", createNumsList(n, 1));

    printf("After adding new contact:\n");
    printAnnuaire(A);
    getchar();

    // --------------

    char *n1[] = {"+112345678900", "+15550123456"};
    addContact(&A, "Peterson", "John", "Colleague", createNumsList(n1, 2));
    char *n2[] = {"+213544332211"};
    addContact(&A, "Chaabane", "Abdelrahmene", "Colleague", createNumsList(n2, 1));
    char *n3[] = {"3015"};
    addContact(&A, "ALG", "Poste", "Service", createNumsList(n3, 1));

    printf("After adding several new contacts:\n");
    printAnnuaire(A);
    getchar();

    // --------------

    contact* foundC = searchContact(A, "Peterson", "John");

    printf("After searching for 'Peterson John' :\n");
    if (foundC == NULL)
        printf("Contact wasn't found.\n");
    else {
        printf("Contact was found:\n");
        printContact(*foundC);
    }
    getchar();

    // --------------

    deleteContact(&A, "ALG", "Poste");

    printf("After deleting 'ALG Poste' contact :\n");
    printAnnuaire(A);
    getchar();

    // --------------

    modifyContact(&A, "Feniche", "Moncef", "Fenniche", "Moncef", "Personal");

    printf("After modifying 'Feniche' contact :\n");
    printf("Feniche -> Fenniche , Me -> Personal\n");
    printAnnuaire(A);
    getchar();

    // --------------

    addContact( &A, "Fenniche", "Moncef", "Personal", createNumsList((char*[]){"+213777001122"}, 1));

    printf("After adding a duplicate contact :\n");
    printAnnuaire(A);
    getchar();

    // --------------

    mergeContacts(&A);

    printf("After merging duplicates :\n");
    printAnnuaire(A);
    getchar();

    // --------------

    Annuaire colA = subContacts(A, "Colleague");

    printf("Sous-Annuaire of 'Colleague': \n");
    printAnnuaire(colA);
    getchar();

    // --------------

    saveAnnuaire(colA, "sous.anr");

    printf("'Colleague' Sous-Annuaire is saved in 'sous.anr'\n");
    getchar();

    loadAnnuaire(&A, "sous.anr");

    printf("'Colleague' Sous-Annuaire is loaded from 'sous.anr'\n");
    printAnnuaire(A);
    getchar();

    return 0;
}
