#include <iostream>
#include <stdio.h>
#include <string.h>
#include <graphics.h>
#include <mmsystem.h>

#define NMAX1 40
#define NMAX2 50
#define NMAX3 5
#define NMAX4 200

using namespace std;

char limba, culoare;
char meniu_elementeRO[11][50]= {"Ampermetru", "Baterie", "Bec", "Dioda", "Generator", "Rezistor", "Siguranta", "Voltmetru", "Fir"};
char meniu_butoaneRO[10][50]= {"Informatii", "Sterge", "Reseteaza", "Salveaza", "Incarca", "Zoom in", "Zoom out", "Roteste"};
char meniu_elementeENG[11][50]= {"Ammeter", "Batery", "LightBulb", "Diode", "Generator", "Resistor", "Fuse", "Voltmeter", "Wire"};
char meniu_butoaneENG[10][50]= {"Information", "Delete", "Reset", "Save", "Load", "Zoom in", "Zoom out", "Rotate"};

char valoare[NMAX2][NMAX2];
char valoares[NMAX2][NMAX2];
int x1, x2, y1, y2, i, j;
int selectat;
unsigned zoom = 25;

void draw_meniu_elemente()
{
    setlinestyle(0, 0, 3);
    settextstyle(3, 0, 2);
    x1=0;
    x2=200;
    if (culoare=='R')
        setbkcolor(COLOR(255, 225, 225));
    else
        setbkcolor(COLOR(230, 255, 255));
    for (i=0, y1=0, y2=70; y1<=560 && y2<=630 && i<9; y1+=70, y2+=70, i++)
    {
        if (mousex()>=x1 && mousex()<=x2 && mousey()>=y1 && mousey()<=y2)
        {
            if (culoare=='R')
                setcolor(13);
            else
                setcolor(3);
            if (limba=='R')
                outtextxy(30,y1+25,meniu_elementeRO[i]);
            else
                outtextxy(30,y1+25,meniu_elementeENG[i]);
        }
        else
        {
            setcolor(0);
            if (limba=='R')
                outtextxy(30,y1+25,meniu_elementeRO[i]);
            else
                outtextxy(30,y1+25,meniu_elementeENG[i]);
            rectangle(x1,y1,x2,y2);
        }
    }
    if (mousex()>=0 && mousex()<=200 && mousey()>=630 && mousey()<=699)
    {
        if (culoare=='R')
                setcolor(13);
            else
                setcolor(3);
        if (limba=='R')
            outtextxy(30,655,"Verifica");
        else
            outtextxy(30,655,"Verify");
    }
    else
    {
        setcolor(0);
        if (limba=='R')
            outtextxy(30,655,"Verifica");
        else
            outtextxy(30,655,"Verify");
        rectangle(0,630,200,699);
    }
}
void draw_meniu_butoane()
{
    setlinestyle(0, 0, 3);
    settextstyle(3, 0, 2);
    x1=800;
    x2=999;
    if (culoare=='R')
        setbkcolor(COLOR(255, 225, 225));
    else
        setbkcolor(COLOR(230, 255, 255));
    setcolor(BLACK);
    rectangle(800,1,999,70);
    for (i=0, y1=70, y2=140; y1<=560 && y2<=630; y1+=70, y2+=70, i++)
    {
        if (mousex()>=x1 && mousex()<=x2 && mousey()>=y1 && mousey()<=y2)
        {
            if (culoare=='R')
                setcolor(13);
            else
                setcolor(3);
            if (limba=='R')
                outtextxy(830,y1+25,meniu_butoaneRO[i]);
            else
                outtextxy(830,y1+25,meniu_butoaneENG[i]);
        }
        else
        {
            setcolor(0);
            if (limba=='R')
                outtextxy(830,y1+25,meniu_butoaneRO[i]);
            else
                outtextxy(830,y1+25,meniu_butoaneENG[i]);
            rectangle(x1,y1,x2,y2);
        }
    }
    if (mousex()>=800 && mousex()<=999 && mousey()>=630 && mousey()<=699)
    {
        if (culoare=='R')
                setcolor(13);
            else
                setcolor(3);
        if (limba=='R')
                outtextxy(830,655,"Valoare");
            else
                outtextxy(830,655,"Value");
    }
    else
    {
        setcolor(0);
        if (limba=='R')
                outtextxy(830,655,"Valoare");
            else
                outtextxy(830,655,"Value");
        rectangle(800,630,999,699);
    }
}

void draw_ecran_circ()
{
    setfillstyle(SOLID_FILL,15);
    bar(240,70,760,630);
}

void draw_bk()
{
    if (culoare=='R')
        setbkcolor(COLOR(255, 225, 225));
    else
        setbkcolor(COLOR(230, 255, 255));
    cleardevice();
}

struct descrierePiesa
{
    unsigned nrComenzi;
    char comanda[NMAX1];
    float x1[NMAX1], y1[NMAX1], x2[NMAX1], y2[NMAX1];
};

struct nodPiesa
{
    float x, y, d;
    bool legat;
};

struct legatura
{
    nodPiesa *nod1, *nod2;
};

struct piesa
{
    char nume[NMAX1], masura[NMAX1];
    int x, y;
    int nrNoduri;
    nodPiesa noduri[NMAX3];
    descrierePiesa descriere;
    float c1x, c1y, c2x, c2y;
    short orientare=0;
};

piesa Piese[NMAX2], Piese_Salvate[NMAX2];
int contor=0, contors=0, contorl=0, contorls=0;
piesa p, nup;
int selectie=0;
nodPiesa *nod1, *nod2;
legatura legaturi[NMAX4], legaturi_salvate[NMAX4];

void initializarePiesa(piesa &p)
{
    FILE* f;
    char numef[NMAX1];
    strcpy(numef, p.nume);
    strcat(numef, ".piesa");
    f=fopen(numef, "rt");
    fscanf(f, "%d\n", &p.descriere.nrComenzi);
    for (unsigned i=1; i<=p.descriere.nrComenzi; i++)
    {
        fscanf(f, "%c", &p.descriere.comanda[i]);
        fscanf(f, "%f %f %f %f \n", &p.descriere.x1[i], &p.descriere.y1[i], &p.descriere.x2[i], &p.descriere.y2[i]);
    }
    fscanf(f, "%d\n", &p.nrNoduri);
    for (unsigned i=1; i<=p.nrNoduri; i++)
    {
        fscanf(f, "%f", &p.noduri[i].d);
        p.noduri[i].x=p.x+zoom*p.noduri[i].d;
        p.noduri[i].y=p.y;
        p.noduri[i].legat=0;
    }
    fscanf(f, "%f %f %f %f \n", &p.c1x, &p.c1y, &p.c2x, &p.c2y);
    fscanf(f, "%s", &p.masura);
    fclose(f);
}

void nuPiesa()
{
    FILE* f;
    char numef[NMAX1];
    strcpy(numef, "nimichihi");
    strcat(numef, ".piesa");
    f=fopen(numef, "rt");
    fscanf(f, "%d\n", &nup.descriere.nrComenzi);
    for (unsigned i=1; i<=nup.descriere.nrComenzi; i++)
    {
        fscanf(f, "%c", &nup.descriere.comanda[i]);
        fscanf(f, "%f %f %f %f \n", &nup.descriere.x1[i], &nup.descriere.y1[i], &nup.descriere.x2[i], &nup.descriere.y2[i]);
    }
    fclose(f);
}

void linie(piesa p, int i)
{
    float x1, x2, y1, y2;
    switch(p.orientare)
    {
    case 0:
    {
        x1=p.descriere.x1[i];
        y1=p.descriere.y1[i];
        x2=p.descriere.x2[i];
        y2=p.descriere.y2[i];
    }
    break;
    case 1:
    {
        x1=p.descriere.y1[i];
        y1=p.descriere.x1[i];
        x2=p.descriere.y2[i];
        y2=p.descriere.x2[i];
    }
    break;
    case 2:
    {
        x1=-p.descriere.x1[i];
        y1=-p.descriere.y1[i];
        x2=-p.descriere.x2[i];
        y2=-p.descriere.y2[i];
    }
    break;
    case 3:
    {
        x1=p.descriere.y2[i];
        y1=-p.descriere.x2[i];
        x2=p.descriere.y1[i];
        y2=-p.descriere.x1[i];
    }
    break;
    }
    line(p.x+zoom*x1, p.y+zoom*y1, p.x+zoom*x2, p.y+zoom*y2);
}

void cerc(piesa p, int i)
{
    float x1, y1, r;
    x1=p.descriere.x1[i];
    y1=p.descriere.y1[i];
    r=p.descriere.x2[i];
    circle(p.x+zoom*x1, p.y+zoom*y1, zoom*r);
}

void dreptunghi(piesa p, int i)
{
    float x1, x2, y1, y2;
    switch(p.orientare)
    {
    case 0:
    {
        x1=p.descriere.x1[i];
        y1=p.descriere.y1[i];
        x2=p.descriere.x2[i];
        y2=p.descriere.y2[i];
    }
    break;
    case 1:
    {
        x1=p.descriere.y1[i];
        y1=p.descriere.x1[i];
        x2=p.descriere.y2[i];
        y2=p.descriere.x2[i];
    }
    break;
    case 2:
    {
        x1=-p.descriere.x1[i];
        y1=-p.descriere.y1[i];
        x2=-p.descriere.x2[i];
        y2=-p.descriere.y2[i];
    }
    break;
    case 3:
    {
        x1=p.descriere.y2[i];
        y1=-p.descriere.x2[i];
        x2=p.descriere.y1[i];
        y2=-p.descriere.x1[i];
    }
    break;
    }
    rectangle(p.x+zoom*x1, p.y+zoom*y1, p.x+zoom*x2, p.y+zoom*y2);
}

void nod(piesa& p, int i)
{
    float x, y;
    switch(p.orientare)
    {
    case 0:
    {
        x=p.x+zoom*p.noduri[i].d;
        y=p.y;
    }
    break;
    case 1:
    {
        x=p.x;
        y=zoom*p.noduri[i].d+p.y;
    }
    break;
    case 2:
    {
        x=p.x-zoom*p.noduri[i].d;
        y=p.y;
    }
    break;
    case 3:
    {
        x=p.x;
        y=p.y-zoom*p.noduri[i].d;
    }
    break;
    }
    p.noduri[i].x=x;
    p.noduri[i].y=y;
    circle(x, y, 5);
}

void deseneazaPiesa(piesa &p)
{
    setlinestyle(0, 0, 1);
    for (unsigned i=1; i<=p.descriere.nrComenzi; i++)
    {
        switch (p.descriere.comanda[i])
        {
        case 'L':
            linie(p, i);
            break;
        case 'C':
            cerc(p, i);
            break;
        case 'D':
            dreptunghi(p, i);
            break;
        }
    }
    for (unsigned i=1; i<=p.nrNoduri; i++)
        nod(p, i);
}

void deseneazaLegatura(nodPiesa *nod1, nodPiesa *nod2)
{
    setlinestyle(0, 0, 1);
    if ((*nod1).x>=(*nod2).x) swap(nod1, nod2);
    if (getpixel((*nod1).x+1, (*nod1).y)!=WHITE)
    {
        if(getpixel((*nod2).x-1, (*nod2).y)!=WHITE)
        {
            line((*nod1).x, (*nod1).y, (*nod1).x, (((*nod1).y+(*nod2).y)/2));
            line((*nod2).x, (*nod2).y, (*nod2).x, (((*nod1).y+(*nod2).y)/2));
            line((*nod1).x, (((*nod1).y+(*nod2).y)/2), (*nod2).x, (((*nod1).y+(*nod2).y)/2));
        }
        else
        {
            line((*nod1).x, (*nod1).y, (*nod1).x, (*nod2).y);
            line((*nod1).x, (*nod2).y, (*nod2).x, (*nod2).y);
        }
    }
    else if(getpixel((*nod1).x-1, (*nod1).y)!=WHITE)
    {
        if(getpixel((*nod2).x+1, (*nod2).y)!=WHITE)
        {
            line((*nod1).x, (*nod1).y, (*nod1).x, (((*nod1).y+(*nod2).y)/2));
            line((*nod2).x, (*nod2).y, (*nod2).x, (((*nod1).y+(*nod2).y)/2));
            line((*nod1).x, (((*nod1).y+(*nod2).y)/2), (*nod2).x, (((*nod1).y+(*nod2).y)/2));
        }
        else if(getpixel((*nod2).x-1, (*nod2).y)!=WHITE)
        {
            line((*nod1).x, (*nod1).y, (*nod2).x, (*nod1).y);
            line((*nod2).x, (*nod1).y, (*nod2).x, (*nod2).y);
        }
        else
        {
            line((*nod1).x, (*nod1).y, (*nod1).x, (*nod2).y);
            line((*nod1).x, (*nod2).y, (*nod2).x, (*nod2).y);
        }
    }
    else
    {
        if (getpixel((*nod2).x+1, (*nod2).y)!=WHITE || getpixel((*nod2).x-1, (*nod2).y)!=WHITE)
        {
            line((*nod1).x, (*nod1).y, (*nod2).x, (*nod1).y);
            line((*nod2).x, (*nod1).y, (*nod2).x, (*nod2).y);
        }
        else
        {
            line((*nod1).x, (*nod1).y, (((*nod1).x+(*nod2).x)/2), (*nod1).y);
            line((((*nod1).x+(*nod2).x)/2), (*nod1).y, (((*nod1).x+(*nod2).x)/2), (*nod2).y);
            line((((*nod1).x+(*nod2).x)/2), (*nod2).y, (*nod2).x, (*nod2).y);
        }
    }
}

void alegePiesa(piesa &p, float y)
{
    int buton;
    for (i=0; i<=490; i+=70)
    {
        if (y>i && y<i+70)
        {
            buton=i/70;
            break;
        }
    }
    strcpy(p.nume, meniu_elementeRO[buton]);
    selectie=1;
}

void salveaza()
{
    for (i=0; i<NMAX2; i++) Piese_Salvate[i]=nup, valoares[i][0]=0;
    for (i=0; i<contor; i++) Piese_Salvate[i]=Piese[i], strcpy(valoares[i],valoare[i]);
    contors=contor;
    for (i=0; i<NMAX4; i++) legaturi_salvate[i].nod1=NULL, legaturi_salvate[i].nod2=NULL;
    for (i=0; i<contorl; i++) legaturi_salvate[i]=legaturi[i];
    contorls=contorl;
}

void incarca()
{
    draw_ecran_circ();
    for (i=0; i<contors; i++)
        deseneazaPiesa(Piese_Salvate[i]);
    for (i=0; i<contorls; i++)
        deseneazaLegatura(legaturi_salvate[i].nod1, legaturi_salvate[i].nod2);
    for (i=0; i<NMAX2; i++) Piese[i]=nup, valoare[i][0]=0;
    for (i=0; i<contors; i++) Piese[i]=Piese_Salvate[i], strcpy(valoare[i],valoares[i]);
    contor=contors;
    for (i=0; i<NMAX4; i++) legaturi[i].nod1=NULL, legaturi[i].nod2=NULL;
    for (i=0; i<contorl; i++) legaturi[i]=legaturi_salvate[i];
    contorl=contorls;
}

void reseteaza()
{
    draw_ecran_circ();
    for (int i=0; i<NMAX2; i++) Piese_Salvate[i]=Piese[i]=nup;
    for (int i=0; i<NMAX2; i++) valoare[i][0]=0;
    for (int i=0; i<NMAX4; i++)
    {
        legaturi[i].nod1=NULL, legaturi[i].nod2=NULL;
        legaturi_salvate[i].nod1=NULL, legaturi_salvate[i].nod2=NULL;
    }
    contorl=contorls=0;
    contor=contors=0;
}

void zoomin(piesa Piese[], int contor, unsigned &zoom)
{
    bool ok=1;
    for (i=0; i<contor; i++)
        if (!(Piese[i].x>=240+(zoom+5)*1.6 && Piese[i].x<=760-(zoom+5)*1.6 && Piese[i].y>=70+(zoom+5)*0.6 && Piese[i].y<=630-(zoom+5)*0.6)) ok=0;
    if (zoom<=65 && ok)
    {
        zoom+=5;
        draw_ecran_circ();
        for (i=0; i<=contor; i++)
            deseneazaPiesa(Piese[i]);
        for (i=0; i<contorl; i++)
            deseneazaLegatura(legaturi[i].nod1, legaturi[i].nod2);
    }
}

void zoomout(piesa Piese[], int contor, unsigned &zoom)
{
    if (zoom>5)
    {
        zoom-=5;
        draw_ecran_circ();
        for (i=0; i<contor; i++)
            deseneazaPiesa(Piese[i]);
        for (i=0; i<contorl; i++)
            deseneazaLegatura(legaturi[i].nod1, legaturi[i].nod2);
    }
}

bool inf=0;
char informatiiRO[NMAX1][NMAX4]= {"- Pentru a adauga o piesa pe ecran, faceti click pe", "butonul din meniu, iar apoi pe ecranul alb.","- Pentru a selecta o piesa faceti click pe ea.", "- Dupa selectare, o piesa poate fi mutata, rotita facand click pe", "butonul `Roteste` sau stearsa facand click pe butonul `Sterge`", "- Butonul `Reseteaza` sterge circuitul curent de pe ecran,", "dar si din memorie", "- Butonul `Salveaza` salveaza circuitul curent.", "- Butonul `Incarca` va incarca ultimul circuit salvat.", "- Pentru a da o valoare unei piese, selectati piesa, iar apoi", "apasati butonul `Valoare`.", "- Pentru a vizualiza valoare unei piese, selectati-o.", "- Pentru a lega doua noduri intre ele, faceti click pe butonul", "`Fir`, iar apoi faceti click pe cele doua noduri.", "- Pentru a sterge un fir, faceti click pe butonul `Fir`, dupa", "pe una dintre extrematile firului, iar apoi pe butonul `Sterge`.", "- Pentru a verifica daca circuitul este valid, faceti click pe", "butonul `Verifica`. Circuitul este valid daca acesta contine o", "baterie sau un generator, si daca toate piesele sunt conectate" ,"si au valori diferite de 0."};
char informatiiENG[NMAX1][NMAX4]= {"- In order to add a piece on the screen click on the menu", "button and then on the white screen.","- In order to select a piece click on it.", "- After selection, a piece can be moved, rotated (by clicking", "on the `Rotate` button) or deleted (by clicking on the `Delete`","button", "- The `Reset` button deletes the current circuit from the", "screen and from memory", "- The `Save` button saves the current circuit.", "- The `Load` button will load the last saved circuit.", "- In order to assign a piece a value, select the piece, then", "click on the `Value` button.", "- In order to view the value of a piece, select it.", "- In order to bind two nodes, click on the `Wire` button and", "then click on the two nodes.", "- In order to delete a wire, click on the `Wire` button, then", "on one of the wire's nodes, then on the `Delete` button.", "- In order to verify if the circuit is valid, click on the`Verify`","button. The circuit is valid if it contains a batery","or a generator, and if all pieces are connected and" ,"have been assigned values greater than 0."};
void open_informatii()
{
    inf=1;
    if (culoare=='R')
        setfillstyle(SOLID_FILL,13);
    else
        setfillstyle(SOLID_FILL,11);
    bar(240,70,760,630);
    setlinestyle(0, 0, 2);
    setcolor(0);
    rectangle(730,71,759,100);
    line(731,71,758,99);
    line(731,99,758,71);
    if (culoare=='R')
        setbkcolor(13);
    else
        setbkcolor(11);
    settextstyle(3, 0, 2);
    if (limba=='R')
        for (int i=0, y=80; i<20; y+=25, i++)
            outtextxy(245,y,informatiiRO[i]);
    else
        for (int i=0, y=80; i<=20; y+=25, i++)
            outtextxy(245,y,informatiiENG[i]);
}

void close_informatii()
{
    inf=0;
    draw_ecran_circ();
    for (int i=0; i<=contor; i++)
        deseneazaPiesa(Piese[i]);
    for (i=0; i<contorl; i++)
        deseneazaLegatura(legaturi[i].nod1, legaturi[i].nod2);
}

void stergeLegatura(legatura legaturi[], nodPiesa* nod, piesa Piese[])
{
    int k;
    for (i=0; i<contorl; i++)
    {
        if(legaturi[i].nod1==nod || legaturi[i].nod2==nod)
        {
            nodPiesa *nod1, *nod2;
            nod1=legaturi[i].nod1;
            nod2=legaturi[i].nod2;
            (*nod1).legat=0;
            (*nod2).legat=0;
            for(k=i; k+1<contorl; k++)
            {
                legaturi[k].nod1=NULL;
                legaturi[k].nod2=NULL;
                legaturi[k].nod1=legaturi[k+1].nod1;
                legaturi[k].nod2=legaturi[k+1].nod2;
            }
            contorl--;
        }
    }
    if (selectie==5)
    {
        draw_ecran_circ();
        for (k=0; k<contor; k++)
            deseneazaPiesa(Piese[k]);
        for (k=0; k<contorl; k++)
            deseneazaLegatura(legaturi[k].nod1, legaturi[k].nod2);
    }
    selectie=0;
}

nodPiesa* selectareNod(int x, int y)
{
    for (i=0; i<contor; i++)
        for (j=1; j<=Piese[i].nrNoduri; j++)
        {
            if (x>=Piese[i].noduri[j].x-5 && x<=Piese[i].noduri[j].x+5 && y>=Piese[i].noduri[j].y-5 && y<=Piese[i].noduri[j].y+5 && Piese[i].noduri[j].legat==0)
            {
                selectie=4;
                nodPiesa *nod;
                nod=&Piese[i].noduri[j];
                Piese[i].noduri[j].legat=1;
                return nod;
            }
            else if(x>=Piese[i].noduri[j].x-5 && x<=Piese[i].noduri[j].x+5 && y>=Piese[i].noduri[j].y-5 && y<=Piese[i].noduri[j].y+5 && Piese[i].noduri[j].legat==1)
            {
                selectie=5;
                nodPiesa *nod;
                nod=&Piese[i].noduri[j];
                return nod;
            }
        }
    selectie=0;
    return NULL;
}

void puneValoare(int x, int y, char mesaj[200], char S[200])
{

    char Enter = 13, BackSpace = 8;
    char MultimeDeCaractereAcceptabile[200] = "0123456789";
    char Sinitial[200], tasta;
    char Ss[200], mesajs[200];

    strcpy(Sinitial,S);
    settextstyle(3, 0, 2);
    setcolor(BLACK);
    strcpy(mesajs,mesaj);
    strcat(mesajs,": ");
    outtextxy(x,y,mesajs);
    x=x+textwidth(mesajs);
    strcpy(S,"");
    do
    {
        tasta=getch();
        if (tasta==0)
            tasta=getch();
        else if (strchr(MultimeDeCaractereAcceptabile,tasta))
        {
            strcpy(Ss,S);
            if (culoare=='R')
                setcolor(COLOR(255,225,225));
            else
                setcolor(COLOR(230, 255, 255));
            outtextxy(x,y,Ss);
            char tt2[2];
            tt2[0]=tasta;
            tt2[1]=0;
            strcat(S,tt2);
            strcpy(Ss,S);
            strcpy(valoare[selectat],Ss);
            setcolor(BLACK);
            outtextxy(x,y,Ss);
        }
        else if (tasta == BackSpace)
        {
            strcpy(Ss,S);
            if (culoare=='R')
                setcolor(COLOR(255,225,225));
            else
                setcolor(COLOR(230, 255, 255));
            outtextxy(x,y,Ss);
            setcolor(BLACK);
            S[strlen(S)-1]=0;
            strcpy(Ss,S);
            strcpy(valoare[selectat],Ss);
            outtextxy(x,y,Ss);
        }
    }
    while (tasta!=Enter);
}

void reset_buton_valoare()
{
    if (culoare=='R')
        setfillstyle(SOLID_FILL,COLOR(255,225,225));
    else
        setfillstyle(SOLID_FILL,COLOR(230, 255, 255));
    bar(802,3,997,68);
    bar(400-textwidth("Introduceti valoarea: "),25,700,60);
}

void sterge(piesa Piese[], int selectat)
{
    int k;
    for (k=1; k<=Piese[selectat].nrNoduri; k++)
    {
        if (Piese[selectat].noduri[k].legat==1)
        {
            nodPiesa* nod;
            nod=&Piese[selectat].noduri[k];
            stergeLegatura(legaturi, nod, Piese);
        }
    }
    for (k=selectat; k<contor; k++)
    {
        for (i=1; i<=Piese[selectat].nrNoduri; i++)
            if (Piese[k].noduri[i].legat==1)
            {
                nodPiesa* nod;
                nod=&Piese[k].noduri[i];
                for (int j=0; j<contorl; j++)
                {
                    if(legaturi[j].nod1==nod)
                        legaturi[j].nod1=&Piese[k-1].noduri[i];
                    else if(legaturi[j].nod2==nod)
                        legaturi[j].nod2=&Piese[k-1].noduri[i];
                }
            }
        Piese[k]=nup;
        valoare[k][0]=0;
        Piese[k]=Piese[k+1];
        strcpy(valoare[k],valoare[k+1]);
    }
    contor--;
    draw_ecran_circ();
    for (k=0; k<contor; k++)
        deseneazaPiesa(Piese[k]);
    for (k=0; k<contorl; k++)
        deseneazaLegatura(legaturi[k].nod1, legaturi[k].nod2);
    selectie=0;
    reset_buton_valoare();
}

int selectarePiesa(int x, int y, piesa Piese[])
{
    for (i=0; i<contor; i++)
        if (x>=Piese[i].x+zoom*Piese[i].c1x && x<=Piese[i].x+zoom+Piese[i].c2x && y>=Piese[i].y+zoom*Piese[i].c1y && y<=Piese[i].y+zoom*Piese[i].c2y)
            return i;
    return -1;
}

void punePiesa(piesa& p, float x, float y)
{
    if (selectarePiesa(x, y, Piese)<0)
    {
        p.x=x;
        p.y=y;
        initializarePiesa(p);
        deseneazaPiesa(p);
        Piese[contor++]=p;
        selectie=0;
    }
}

void roteste(piesa &p)
{
    selectie=0;
    p.orientare=(p.orientare+1)%4;
    draw_ecran_circ();
    setcolor(0);
    for (int i=0; i<=contor; i++)
        deseneazaPiesa(Piese[i]);
    for (int k=0; k<contorl; k++)
        deseneazaLegatura(legaturi[k].nod1, legaturi[k].nod2);
    reset_buton_valoare();
}

void mutaPiesa(piesa &p, float x, float y)
{
    if (selectarePiesa(x, y, Piese)<0)
    {
        p.x=x;
        p.y=y;
        draw_ecran_circ();
        for (int i=0; i<=contor; i++)
            deseneazaPiesa(Piese[i]);
        for (int k=0; k<contorl; k++)
            deseneazaLegatura(legaturi[k].nod1, legaturi[k].nod2);
        selectie=0;
        reset_buton_valoare();
    }
}

void verificare()
{
    bool baterie=0, inchis=1, val=1;
    for (i=0; i<contor; i++)
    {
        if (strcmp(Piese[i].nume, "Baterie")==0 || strcmp(Piese[i].nume, "Generator")==0 || strcmp(Piese[i].nume, "Batery")==0)
            baterie=1;
        for (j=1; j<=Piese[i].nrNoduri; j++)
            if (Piese[i].noduri[j].legat==0)
                inchis=0;
        if (!valoare[i][0]) val=0;
        if (inchis==0 || val==0)
            break;
    }
    if (baterie==1 && inchis==1 && val==1)
    {
        if (limba=='R')
            outtextxy(240, 640, "Felicitari! Circuitul este valid.");
        else
            outtextxy(240, 640, "Congratulations! The circuit is valid.");
        if (culoare=='R')
            readimagefile("pisica.jpg", 660, 635, 750, 700);
        else
            readimagefile("pisica_albastra.jpg", 660, 635, 750, 700);
        draw_ecran_circ();
        setcolor(GREEN);
        for (int i=0; i<=contor; i++)
            deseneazaPiesa(Piese[i]);
        for (int k=0; k<contorl; k++)
            deseneazaLegatura(legaturi[k].nod1, legaturi[k].nod2);
        setcolor(0);
    }
    else
    {
        if (limba=='R')
            outtextxy(240, 640, "Ne pare rau! Circuitul nu este valid. Mai incearca!");
        else
            outtextxy(240, 640, "Sorry! The circuit is not valid. Try again!");
        draw_ecran_circ();
        setcolor(COLOR(255, 0, 0));
        for (int i=0; i<=contor; i++)
            deseneazaPiesa(Piese[i]);
        for (int k=0; k<contorl; k++)
            deseneazaLegatura(legaturi[k].nod1, legaturi[k].nod2);
        setcolor(0);
    }
}

void procesareClick()
{
    char S[200];
    bool click=false;
    int x, y;
    do
    {
        if (ismouseclick(WM_MOUSEMOVE))
        {
            x = mousex();
            y = mousey();
        }
        if (ismouseclick(WM_LBUTTONDOWN) && !click)
        {
            if (selectie==6)
            {
                if (culoare=='R')
                    setfillstyle(SOLID_FILL, COLOR(255,225,225));
                else
                    setfillstyle(SOLID_FILL, COLOR(230, 255, 255));
                bar(240, 640, 240+textwidth("Ne pare rau! Circuitul nu este valid. Mai incearca!"), 665);
                bar(660, 635, 750, 700);
                draw_ecran_circ();
                for (int i=0; i<=contor; i++)
                    deseneazaPiesa(Piese[i]);
                for (int k=0; k<contorl; k++)
                    deseneazaLegatura(legaturi[k].nod1, legaturi[k].nod2);
                selectie=0;
            }
            getmouseclick(WM_LBUTTONDOWN, x, y);
            if (!inf && x >= 800 && x <= 999 && y>=420 && y<=490)
                zoomin(Piese, contor, zoom);
            else if (!inf && x >= 800 && x <= 999 && y>=490 && y<=560)
                zoomout(Piese, contor, zoom);
            else if (x >= 800 && x <= 999 && y >= 70 && y<=140)
                open_informatii();
            else if (x >= 731 && x <= 758 && y >= 71 && y<=100)
                close_informatii();
            else if (!inf && x >= 800 && x <= 999 && y >= 280 && y<=350)
                salveaza();
            else if (!inf && x >= 800 && x <= 999 && y >= 350 && y<=420)
                incarca();
            else if (!inf && x >= 800 && x <= 999 && y >= 210 && y<=280)
                reseteaza();
            else if (!inf && selectie==0 && x >= 0 && x <= 200 && y<=560)
                alegePiesa(p, y);
            else if (!inf && selectie==1 && x>=240+zoom*1.55 && x<=760-zoom*1.55 && y>=70+zoom*0.5 && y<=630-zoom*0.5)
                punePiesa(p, x, y);
            else if (!inf && selectie==2 && x>=800 && x<=999  && y>=140 && y<=210)
                sterge(Piese, selectat);
            else if (!inf && selectie==2 && x>=800 && x<=999 && y>=560 && y<=630)
                roteste(Piese[selectat]);
            else if (!inf && selectie==2 && x>=240+zoom*1.55 && x<=760-zoom*1.55 && y>=70+zoom*0.5 && y<=630-zoom*0.5)
                mutaPiesa(Piese[selectat], x, y);
            else if (!inf && selectie==2 && x>=800 && x<=999 && y>=630 && y<=699)
            {
                if (limba=='R')
                    puneValoare(240,25,"Introduceti valoarea",S);
                else
                    puneValoare(240,25,"Value input",S);
                outtextxy(830,25,S);
                reset_buton_valoare();
                selectie=0;
                deseneazaPiesa(Piese[selectat]);
            }
            else if (selectie==0 && x>=240 && x<=760 && y>=70 && y<=630)
            {
                selectat=selectarePiesa(x, y, Piese);
                if (selectat!=-1 && selectat!=contor)
                {
                    selectie=2;
                    if (culoare=='R')
                        setcolor(13);
                    else
                        setcolor(3);
                    deseneazaPiesa(Piese[selectat]);
                    setcolor(BLACK);
                    strcat(valoare[selectat]," ");
                    strcat(valoare[selectat],Piese[selectat].masura);
                    char s[NMAX1];
                    strcpy(s,"0 ");
                    strcat(s,Piese[selectat].masura);
                    if (strchr("0123456789",valoare[selectat][0]))
                        outtextxy(830,25,valoare[selectat]);
                    else
                        outtextxy(830,25,s);
                }
                for (int i=0; i<strlen(valoare[selectat]); i++)
                    if (!strchr("0123456789",valoare[selectat][i])) valoare[selectat][i]=0;
            }
            else if (selectie==0 && y>=560 && y<=630 && x>=0 && x<=200)
            {
                selectie=3;
                clearmouseclick(WM_LBUTTONDOWN);
            }
            else if (selectie==3 && x>=240 && x<=760 && y>=70 && y<=630)
                nod1=selectareNod(x, y);
            else if ((selectie==4 || selectie==5) && x>=240 && x<=760 && y>=70 && y<=630)
            {
                if (selectie!=5)
                    nod2=selectareNod(x, y);
                if (nod2==NULL) selectie=5;
                bool ok=1;
                for (i=0; i<contor && selectie==4; i++)
                {
                    nodPiesa *nodp1, *nodp2;
                    nodp1=&Piese[i].noduri[1];
                    nodp2=&Piese[i].noduri[2];
                    if ((nodp1==nod1 && nodp2==nod2) || (nodp2==nod1 && nodp1==nod2))
                    {
                        ok=0;
                        break;
                    }
                }
                if (selectie==4 && ok==1)
                {
                    deseneazaLegatura(nod1, nod2);
                    legaturi[contorl].nod1=nod1;
                    legaturi[contorl++].nod2=nod2;
                }
                else
                {
                    (*nod1).legat=0;
                    (*nod2).legat=0;
                    nod1=NULL;
                    nod2=NULL;
                }
                selectie=0;
            }
            else if (selectie==5 && x>=800 && x<=999 && y>=140 && y<=210)
                stergeLegatura(legaturi, nod1, Piese);
            else if (!inf && selectie==0 && x>0 && x<200 && y>=630 && y<700)
            {
                verificare();
                selectie=6;
            }
            else
                clearmouseclick(WM_LBUTTONDOWN);
            click=true;
        }
    }
    while(!click);
}

int main()
{
    cout<<"Introduceti 'R' pentru limba romana sau 'E' pentru limba engleza: ";
    cin>>limba;
    if (limba=='R')
        cout<<"Introduceti 'A' pentru tema albastra sau 'R' pentru tema roz: ";
    else
        cout<<"Input 'A' for the blue theme or 'R' for the pink theme: ";
    cin>>culoare;
    int gd=DETECT, gm;
    initgraph(&gd, &gm, "");
    initwindow(1000, 700, "Electron");
    draw_bk();
    draw_ecran_circ();
    while (1)
    {
        draw_meniu_elemente();
        draw_meniu_butoane();
        if (ismouseclick(WM_LBUTTONDOWN))
        {
            PlaySound(TEXT("sunet.wav"), NULL, SND_FILENAME | SND_ASYNC);
            procesareClick();
        }
    }
    getch();
    closegraph();
    return 0;
}
