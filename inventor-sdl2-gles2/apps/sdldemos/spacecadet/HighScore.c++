/*
 * Copyright 1994, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to restrictions
 * as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
 * and Computer Software clause at DFARS 252.227-7013, and/or in similar or
 * successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
 * rights reserved under the Copyright Laws of the United States.
 */
#include <string.h>
#include <Inventor/SbPList.h>
#include "Common.h"
#include "HighScore.h"

// Temporary file for writing
#define TMP_SCORE_FILE	"/tmp/SpaceCadet.tmpscores"

// Pointers to instances of this struct are stored in the "scores" SbPList.
struct Score {
    const char	*name;
    int		level;
    int		score;
};

const char	*HighScore::scoreFileName;
SbPList		*HighScore::scores;
int		 HighScore::numScores;

////////////////////////////////////////////////////////////////////////////
//
// Initializes class, given name of high score file
//
////////////////////////////////////////////////////////////////////////////

void
HighScore::init(const char *fileName)
{
    scoreFileName = fileName;
    numScores = 0;
    scores = new SbPList(NUM_HIGH_SCORES);

    FILE *fp = fopen(scoreFileName, "r");

    // If file does not exist, no scores to read
    if (fp == NULL)
	return;

    SbBool	ok = TRUE;
    int		n;

    // Read number of scores from file
    if (fread(&n, sizeof(n), 1, fp) == 1) {

	numScores = n;

	// Read scores from file
	for (int i = 0; i < numScores; i++) {
	    char nameBuf[256];
	    int	 lev, sc;

	    // Read size of name, name, level, and score
	    if (fread(&n,       sizeof(n),    1, fp) != 1 ||
		fread(&nameBuf, sizeof(char), n, fp) != n ||
		fread(&lev,     sizeof(lev),  1, fp) != 1 ||
		fread(&sc,      sizeof(sc),   1, fp) != 1) {
		ok = FALSE;
		break;
	    }

	    // Set up score
	    Score *score = new Score;
	    score->name = strdup(nameBuf);
	    score->level = lev;
	    score->score = sc;
	    scores->append(score);
	}
    }

    if (! ok) {
	fprintf(stderr, "Score file (\"%s\") is corrupt\n", scoreFileName);
	for (int i = 0; i < scores->getLength(); i++)
	    delete (Score *) (*scores)[i];
	delete scores;
	scores = NULL;
	numScores = 0;
    }

    fclose(fp);
}

////////////////////////////////////////////////////////////////////////////
//
// Adds the given score to the current list. Returns the index of
// the score if it is now a high score. Returns -1 otherwise.
//
////////////////////////////////////////////////////////////////////////////

int
HighScore::addScore(const char *name, int level, int score)
{
    int i;

    if (numScores > 0) {

	// See which of the high scores this is higher than
	for (i = 0; i < numScores; i++) {
	    Score *sc = (Score *) (*scores)[i];
	    if (score > sc->score)
		break;
	}

	// Not higher than any score, but there may still be room
	if (i == numScores && numScores >= NUM_HIGH_SCORES)
	    return -1;
    }
    else
	i = 0;

    // Insert the new score
    Score *sc = new Score;
    sc->name = name;
    sc->level = level;
    sc->score = score;
    scores->insert(sc, i);

    // Make sure there aren't too many
    if (++numScores > NUM_HIGH_SCORES) {
	scores->truncate(NUM_HIGH_SCORES);
	numScores = NUM_HIGH_SCORES;
    }

    return i;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns the number of scores in the high score list
//
////////////////////////////////////////////////////////////////////////////

int
HighScore::getNumScores()
{
    return numScores;
}

////////////////////////////////////////////////////////////////////////////
//
// Returns the values for the i'th high score
//
////////////////////////////////////////////////////////////////////////////

void
HighScore::getScore(int i, const char *&name, int &level, int &score)
{
    Score *sc = (Score *) (*scores)[i];

    name = sc->name;
    level = sc->level;
    score = sc->score;
}

////////////////////////////////////////////////////////////////////////////
//
// Write the scores back to the score file
//
////////////////////////////////////////////////////////////////////////////

void
HighScore::write()
{
    // Open a temporary file for writing
    FILE *fp = fopen(TMP_SCORE_FILE, "w");

    if (fp == NULL) {
	fprintf(stderr, "Can't open temporary file (\"%s\") for writing\n",
		TMP_SCORE_FILE);
	return;
    }

    // Write number of scores
    fwrite(&numScores, sizeof(numScores), 1, fp);

    // Write scores to file
    for (int i = 0; i < numScores; i++) {

	Score	*sc = (Score *) (*scores)[i];
	int	len = strlen(sc->name) + 1;		// Get NUL char

	fwrite(&len,       sizeof(len),       1,   fp);
	fwrite(sc->name,   sizeof(char),      len, fp);
	fwrite(&sc->level, sizeof(sc->level), 1,   fp);
	fwrite(&sc->score, sizeof(sc->score), 1,   fp);
    }

    fclose(fp);

    // Move temporary file to real score file
    char buf[1024];
    sprintf(buf, "/bin/mv %s %s", TMP_SCORE_FILE, scoreFileName);
    system(buf);
}
