
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Kamil Jeřábek, září 2018
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu před prvním použitím,
**      DisposeList ... zrušení všech prvků seznamu,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int errflg;
int solved;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
	L->Act = NULL;
	L->First = NULL;
}

void DisposeList (tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/
	while (L->First != NULL)	/*Cyklus na zrusenie vsetkych prvkov*/
	{
		if (L->Act == L->First) L->Act = NULL;	/*Zrusenie aktivneho prvku*/
		tElemPtr item = L->First;	/*Prvy prvok pripraveny na odstranenie*/
		L->First = item->ptr;	/*Nastavenie dalsieho prvku ako prveho*/
		free(item);	/*Uvolnenie prvku*/
	}
}

void InsertFirst (tList *L, int val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/
	tElemPtr item = malloc(sizeof(struct tElem));	/*Vytvorenie prvku*/
	if (item == NULL)	/*Ak nastane chyba pri alokacii tak sa nevlozi prvok*/
	{
		Error();
	}
	item->data = val;	/*Nastavenie hodnoty*/
	item->ptr = L->First;	/*Ukazatel na zaciatok*/
	L->First = item;	/*Ukazatel na zaciatok ukazuje na novo vytvoreny prvok*/
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/
	L->Act = L->First;	/*Aktivita na prvy prvok*/
}

void CopyFirst (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
	if (L->First != NULL)	/*Vratenie hodnoty iba pokial zoznam nie je prazdny*/
	{
		*val = L->First->data;	/*Vratenie hodnoty prveho prvku zoznamu*/
	}
	else Error();	/*Prazdny zoznam*/
}

void DeleteFirst (tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
	if (L->First != NULL)	/*Kontrola neprazdneho zoznamu*/
	{
		tElemPtr item = L->First;	/*Prpravenie prveho prvku na zrusenie*/
		if (L->First == L->Act)	/*Ruseny prvok je zaroven aktivny*/
		{
			L->Act = NULL;	/*Zrusenie aktivity zoznamu*/
		}
		L->First = item->ptr;	/*Prvym prvkom sa stava nasledujuci*/
		free(item);	/*Uvolnenie pouzivanej pamate*/
	}
}	

void PostDelete (tList *L) {
/* 
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/
	tElemPtr item;
	if (L->Act != NULL && L->Act->ptr != NULL)	/*Zrusi prvok iba ak existuje dalsi prvok*/
	{
		item = L->Act->ptr;	/*Ukazatel na ruseneho*/
		L->Act->ptr = item->ptr;	/*Ukazatel prvku ktory bude po novom nasledovat aktivny prvok*/
		free(item);
	}
}

void PostInsert (tList *L, int val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/
	if (L->Act != NULL)	/*Operacia iba pre aktivny zoznam*/
	{
		tElemPtr item = malloc(sizeof(struct tElem));
		if (item == NULL)	/*Chyba pri alokacii*/
		{
			Error();
		}
		item->data = val;	/*Nastavenie hodnoty*/
		item->ptr = L->Act->ptr;	/*Ukazatel na nasledujuci aktivny*/
		L->Act->ptr = item; /*Prvok je nasledujuci aktivny*/
	}
}

void Copy (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
	if (L->Act != NULL) /*Ak je aktivny vykona operacie v opacnom pripade zavola Error()*/
	{
		*val = L->Act->data;
	}
	else Error();
}

void Actualize (tList *L, int val) {
/*
** Přepíše data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/
	if (L->Act != NULL)
	{
		L->Act->data = val;
	}
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
	if (L->Act != NULL)
	{
		if (L->Act->ptr != NULL)	/*Kontrola ci prvok nie je posledny v zozname*/
		{
			L->Act = L->Act->ptr;	/*Posunutie aktivity na dalsi prvok*/
		}
		else
		{
			L->Act = NULL;	/*Ak aktivny prvok je posledny v zozname tak sa zoznam stane neaktivnym*/
		}
	}
}

int Active (tList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return. 
**/
	return !(L->Act == NULL);
}

/* Konec c201.c */
