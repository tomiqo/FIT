
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu,
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek,
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku,
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu,
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem,
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu,
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int errflg;
int solved;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	while (L->Last != NULL)	/*Prechadzanie kazdeho prvku od konca*/
	{
		if (L->Last == L->Act)	/*Pokial rusim aktivny prvok*/
		{
			L->Act = NULL;
		}
		if (L->Last == L->First)	/*Pri ruseni posledneho prvku v zozname nastavim ukazatel na prvy prvok na NULL*/
		{
			L->First = NULL;
		}
		tDLElemPtr item = L->Last;	/*Pomocna premenna pre funkciu free()*/
		L->Last = L->Last->lptr;	/*Ako dalsi prvok nastavim o jeden prvok do lava*/
		free(item);	/*Uvolnenie ruseneho prvku*/
	}
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr item = malloc(sizeof(struct tDLElem));	/*Kontrola uspesnosti alokacie pomocou malloc*/
	if (item != NULL)
	{
		item->data = val;	/*Hodnota noveho prvku*/
		item->lptr = NULL;	/*Lavy ukazatel noveho prvku ukazuje na NULL*/
		item->rptr = L->First;	/*Pravy noveho prvku ukazuje na prvy prvok*/
		if (L->First != NULL)	/*V pripade ze neukladam prvy,prvy prvok do zoznamu*/
		{
			L->First->lptr = item;	/*Prvy bude ukazovat na novy prvok*/
		}
		else	/*Zoznam je prazdny a ide tak o prve vlozenie*/
		{
			L->Last = item;	/*Novy prvok bude posledny*/
		}
		L->First = item;	/*Ukazatel na zaciatok bude ukazovat na novy prvok*/
	}
	else	/*Nedostatok pamati pre novy prvok -chyba*/
	{
		DLError();
	}
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr item = malloc(sizeof(struct tDLElem));	/*Kontrola uspesnosti alokacie pomocou malloc*/
	if (item != NULL)
	{
		item->data = val;	/*Hodnota noveho prvku*/
		item->lptr = L->Last;	/*Ukazatel noveho prvku do lava bude ukazovat na posledny prvok*/
		item->rptr = NULL;	/*Ukazatel ukazuje na NULL nakolko z neho bude posledny prvok*/
		if (L->Last != NULL)	/*V pripade ze neukladam prvy posledny prvok do zoznamu*/
		{
			L->Last->rptr = item;	/*Posledny bude ukazovat na novy prvok*/
		}
		else	/*Zoznam je prazdny a ide tak o prve vlozenie*/
		{
			L->First = item;	/*Novy prvok bude aj prvy*/
		}
		L->Last = item;	/*Ukazatel na zaciatok bude ukazovat na novy prvok*/
	}
	else	/*Nedostatok pamati pre novy prvok -chyba*/
	{
		DLError();
	}
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if (L->First != NULL)	/*Vratenie hodnoty iba neprazdneho zoznamu*/
	{
		*val = L->First->data;	/*Vratenie hodnoty prveho prvku*/
	}
	else
	{
		DLError();
	}
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if (L->First != NULL)	/*Vratenie hodnoty iba neprazdneho zoznamu*/
	{
		*val = L->Last->data;	/*Vratenie hodnoty posledneho prvku*/
	}
	else
	{
		DLError();
	}
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	tDLElemPtr item;
	if (L->First != NULL)	/*Pri prazdnom zozname sa nevykona nic*/
	{
		item = L->First;
		if (L->Act == L->First)	/*Zrusenie aktivity ak prvy prvok bol aktivny*/
		{
			L->Act = NULL;
		}
		if (L->First == L->Last)	/*Zrusenie pokial sa jednalo o jediny prvok zoznamu*/
		{
			L->First = NULL;
			L->Last = NULL;
		}
		else
		{
			L->First = L->First->rptr;	/*Novy zaciatok zoznamu*/
			L->First->lptr = NULL;	/*Ukazatel prveho dolava na NULL*/
		}
		free(item);
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if (L->First != NULL)	/*Pri prazdnom zozname sa nevykona nic*/
	{
		tDLElemPtr item = L->Last;
		if (L->Act == L->Last)	/*Zrusenie aktivity ak posledny prvok bol aktivny*/
		{
			L->Act = NULL;
		}
		if (L->First == L->Last)	/*Zrusenie pokial sa jednalo o jediny prvok zoznamu*/
		{
			L->First = NULL;
			L->Last = NULL;
		}
		else
		{
			L->Last = L->Last->lptr;	/*Novy koniec zoznamu*/
			L->Last->rptr = NULL;	/*Ukazatel od noveho posledneho prvku do prava na NULL*/
		}
		free(item);
	}
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if ((L->Act != NULL) && (L->Act != L->Last))	/*Zrusi prvok zoznamu iba ak je aktivny a aktivnym nie je posledny prvok*/
	{
		tDLElemPtr item = L->Act->rptr;	/*Ukazatel na nasledujuci prvok za aktivnym*/
		L->Act->rptr = item->rptr;	/*Ukazatel na prvok do prava bude ukazovat o jeden prvok dalej*/
		if (L->Last == item)	/*Pokial rusim posledny prvok nastavim aktivny ako posledny*/
		{
			L->Last = L->Act;
		}
		else
		{
			item->rptr->lptr = L->Act;	/*V opacnom pripade prvok za zrusenym musi ukazovat do lava na aktivny prvok*/
		}
		free(item);
	}
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if ((L->Act != NULL) && (L->Act != L->First))	/*Zrusi prvok zoznamu iba ak je aktivny a aktivnym nie je prvy prvok*/
	{
		tDLElemPtr item = L->Act->lptr; /*Ukazatel na predchadzajuci prvok pred aktivnym*/
		L->Act->lptr = item->lptr;	/*Ukazatel na prvok do lava od aktivneho bude ukazovat o jeden prvok dalej*/
		if (L->First == item)	/*Pokial rusim prvy prvok nastavim aktivny ako prvy*/
		{
			L->First = L->Act;
		}
		else
		{
			item->lptr->rptr = L->Act;	/*V opacnom pripade prvok pred rusenim bude ukazovat do prava na aktivny prvok*/
		}
		free(item);
	}
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL)	/*Kontrola ci je zoznam aktivny*/
	{
		tDLElemPtr item = malloc(sizeof(struct tDLElem));	/*Kontrola uspesnosti alokacie pomocou malloc*/
		if (item != NULL)
		{
			item->data = val;
			item->rptr = L->Act->rptr;	
			item->lptr = L->Act;
			L->Act->rptr = item;
			if (L->Act == L->Last)	/*Pripad ze vkladam prvok za posledneho*/
			{
				L->Last = item;	/*Novy posledny prvok*/
			}
			else
			{
				item->rptr->lptr = item;	/*Naviazanie praveho susedneho prvku na novy vlozeny prvok*/
			}
		}
		else	/*Nedostatok pamati pre novy prvok -chyba*/
		{
			DLError();
		}
	}
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if (L->Act != NULL)	/*Kontrola ci je zoznam aktivny*/
	{
		tDLElemPtr item = malloc(sizeof(struct tDLElem));	/*Kontrola uspesnosti alokacie pomocou malloc*/
		if (item != NULL)
		{
			item->data = val;
			item->rptr = L->Act;
			item->lptr = L->Act->lptr;
			L->Act->lptr = item;
			if (L->Act == L->First)	/*Pripad ze vkladam prvok pred prvy*/
			{
				L->First = item;	/*Novy prvy prvok*/
			}
			else
			{
				item->lptr->rptr = item;	/*Naviazanie laveho susedneho prvku na novo vlozeny prvok*/
			}
		}
		else	/*Nedostatok pamati pre novy prvok -chyba*/
		{
			DLError();
		}
	}
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if (L->Act != NULL)
	{
		*val = L->Act->data;
	}
	else
	{
		DLError();
	}
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if (L->Act != NULL)
	{
		L->Act->data = val;
	}
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL)
	{
		L->Act = L->Act->rptr;
	}
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	if (L->Act != NULL)
	{
		L->Act = L->Act->lptr;
	}
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return(L->Act != NULL);
}

/* Konec c206.c*/
