/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef CHICANE_CARD_GAME_H
#define CHICANE_CARD_GAME_H 


#include "patiencecardgame.h"


class ChicaneFaceDownDeck : public PatienceFaceDownDeck
{
public:
    ChicaneFaceDownDeck(int x, int y, QCanvas *canvas) :
        PatienceFaceDownDeck(x, y, canvas) { }

};


class ChicaneDiscardPile : public PatienceDiscardPile
{
public:
    ChicaneDiscardPile(int x, int y, QCanvas *canvas) :
        PatienceDiscardPile(x, y, canvas) { }

};


class ChicaneWorkingPile :  public PatienceWorkingPile 
{
public:
    ChicaneWorkingPile(int x, int y, QCanvas *canvas) :
	PatienceWorkingPile(x, y, canvas) { }

    virtual bool isAllowedOnTop(Card *card) {
	if ( card->isFacing() &&
//	     ( ( ( cardOnTop() == NULL ) && (card->getValue() == king) ) ||		// diese Zeile sorgt daf�r dass nur Kings auf leere Pl�tze d�rfen
	     ( (cardOnTop() == NULL) ||							// auf einen Freiplatz darf alles!
	       ( (cardOnTop() != NULL) &&
 	         ((int)card->getValue() + 1 == (int)cardOnTop()->getValue()) &&
	         (card->isRed() != cardOnTop()->isRed()) ) ) )
	    return TRUE;
        return FALSE;	
    }
    virtual bool isAllowedToBeMoved(Card *card) {
	if (!card->isFacing()) return FALSE;

        int nextExpectedValue = (int)card->getValue();
        bool nextExpectedColor = card->isRed();

        while ((card != NULL)) {
            if ( (int)card->getValue() != nextExpectedValue )
                return FALSE;
            if ( card->isRed() != nextExpectedColor )
                return FALSE;
            nextExpectedValue--;;
            nextExpectedColor = !nextExpectedColor;
            card = cardInfront(card);
        }
        return TRUE;
    }

    virtual void cardRemoved(Card *card) {
	Q_UNUSED(card);

	Card *newTopCard = cardOnTop();

	if ( !newTopCard ) {
	    top = QPoint( pileX, pileY );
	    setNextX( pileX );
    	    setNextY( pileY );
	    return;
	} else {
	    top = getCardPos(NULL);
	    if ( newTopCard->isFacing() == FALSE ) {
		int offsetDown = ( qt_screen->deviceWidth() < 200 ) ? 9 : 13;
		// correct the position taking in to account the card is not
    		// yet flipped, but will become flipped
    		top = QPoint( top.x(), top.y() - 3 );		// Keine Verschiebung!
		newTopCard->flipTo( top.x(), top.y() );
		top = QPoint( top.x(), top.y() + offsetDown );
	    }
    	    setNextX( top.x() );
	    setNextY( top.y() );
	}
    }
    virtual QPoint getCardPos(Card *c) {
	int x = pileX, y = pileY;
	Card *card = cardOnBottom();
	while ((card != c) && (card != NULL)) {
	    if (card->isFacing()) {
		int offsetDown = ( qt_screen->deviceWidth() < 200 ) ? 9 : 13;
		y += offsetDown; 
	    } else {
		x += 0;   					// Keine Verschiebung! 
		y += 3;
	    }
	    card = cardInfront(card); 
	}
	return QPoint( x, y );
    }

    virtual QPoint getHypertheticalNextCardPos(void) {
//        return top;
        return QPoint( getNextX(), getNextY() );
    }

private:
    QPoint top;

};


class ChicaneCardGame : public CanvasCardGame
{
public:
    ChicaneCardGame(QCanvas *c, bool snap, QWidget *parent = 0);
//    virtual ~ChicaneCardGame();
    virtual void deal(void);
    virtual bool haveWeWon() { 
	return ( discardPiles[0]->kingOnTop() &&
		 discardPiles[1]->kingOnTop() &&
		 discardPiles[2]->kingOnTop() &&
		 discardPiles[3]->kingOnTop() &&
		 discardPiles[4]->kingOnTop() &&
		 discardPiles[5]->kingOnTop() &&
		 discardPiles[6]->kingOnTop() &&
		 discardPiles[7]->kingOnTop() );;
    }
    virtual void mousePress(QPoint p);
    virtual void mouseRelease(QPoint p) { Q_UNUSED(p); }
//    virtual void mouseMove(QPoint p);
    virtual bool mousePressCard(Card *card, QPoint p);
    virtual void mouseReleaseCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
//    virtual void mouseMoveCard(Card *card, QPoint p) { Q_UNUSED(card); Q_UNUSED(p); }
    bool canTurnOverDeck(void) { return (FALSE); }
    void throughDeck(void) { }
    bool snapOn;
    void writeConfig( Config& cfg );
    void readConfig( Config& cfg );
private:
    ChicaneWorkingPile *workingPiles[8];
    ChicaneDiscardPile *discardPiles[8];
    ChicaneFaceDownDeck *faceDownDealingPile;
};


#endif

