////////////////////////////////////////////////////////////////////////////
//
// Class: HighScore
//
// Static class that manages high score file.
//
////////////////////////////////////////////////////////////////////////////

extern class SbPList;

class HighScore {
  public:
    // Initializes class, given name of high score file
    static void		init(const char *fileName);

    // Adds the given score to the current list. Returns the index of
    // the score if it is now a high score. Returns -1 otherwise.
    static int		addScore(const char *name, int level, int score);

    // Returns the number of scores in the high score list
    static int		getNumScores();

    // Returns the values for the i'th high score
    static void		getScore(int i,
				 const char *&name, int &level, int &score);

    // Write the scores back to the score file
    static void		write();

  private:

    static const char	*scoreFileName;
    static SbPList	*scores;
    static int		numScores;
};
