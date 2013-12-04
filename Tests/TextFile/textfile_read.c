#include <RFtextfile.h>
#include <RFstring.h>
#include <refu.h>
#include "../refu_tests.h"

int main()
{
    struct RFtextfile f;
    struct RFstringx line;

    DEFAULT_LIB_INIT();
    //initialize the buffer
    EXPECT(true,rf_stringx_init_buff(&line,4096,""));

    //utf8 file
    EXPECT(true,
           rf_textfile_init(&f, RFS_("utf8textfile"),
                           RF_FILE_READ, RF_ENDIANESS_UNKNOWN,
                           RF_UTF8, RF_EOL_LF));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true, rf_string_equal(&line, RFS_("This is a UTF-8 File")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("【男子は必見】女子力MAXのイケてる女子が語る "
                               "/ デートで電車を使う男は女子にとって迷惑だ"
                               "から一生独身でいてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("よかったー！うちの地域はJRがディーゼル機関"
                               "車だから電車じゃないわー！ 【男子は必見】女"
                               "子力MAXのイケてる女子が語る / デートで電車"
                               "を使う男は女子にとって迷惑だから一生独身で"
                               "いてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει απ"
                    "ό την απαίτησή του για άμεσο \"κούρεμα\" του"
                    " ελληνικού χρέους, ώστε να αποκατασταθεί η β"
                    "ιωσιμότητά του, υπό την προϋπόθεση όμως ότι "
                    "η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό "
                    "του ελληνικού προγράμματος με αξιόπιστα "
                    "μέτρα.")));
    EXPECT(RF_SUCCESS,rf_textfile_read_line_chars(&f, &line,87));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("河村氏は２０日、東京都内で維新の石原代表と会"
                    "談した。同席者によると、石原氏からはその場で"
                    "、来月４日に公示される衆院選までに維新と合流"
                    "することは困難との見通しが示されたという。")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφ"
                    "ε την Κυριακή ο Carmela Avner, Chief Informa"
                    "tion Officer της ισραηλινής κυβέρνησης. «Το "
                    "πρώτο είναι το πραγματικό πεδίο της μάχης, τ"
                    "ο δεύτερο είναι τα social media και το τρίτο"
                    " είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπ"
                    "αθούν να παρεμποδίσουν την πρόσβαση στις κρα"
                    "τικές ιστοσελίδες σε καθημερινή βάση. Επομέν"
                    "ως, αυτή τη χρονική στιγμή, είναι εξαιρετικά"
                    " σημαντικό να προστατεύσουμε τα ηλεκτρονικά "
                    "μας συστήματα».")));
    EXPECT(RE_FILE_EOF, rf_textfile_read_line(&f, &line));
    rf_textfile_deinit(&f);

	//utf16 le file
    EXPECT(true, rf_textfile_init(&f, RFS_("utf16letextfile"),
                                 RF_FILE_READ, RF_LITTLE_ENDIAN,
                                 RF_UTF16, RF_EOL_LF));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f,&line));
    EXPECT(true, rf_string_equal(&line,
                                RFS_("This is a Little Endian UTF-16 File")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("【男子は必見】女子力MAXのイケてる女子が語る "
                               "/ デートで電車を使う男は女子にとって迷惑だ"
                               "から一生独身でいてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("よかったー！うちの地域はJRがディーゼル機関"
                               "車だから電車じゃないわー！ 【男子は必見】女"
                               "子力MAXのイケてる女子が語る / デートで電車"
                               "を使う男は女子にとって迷惑だから一生独身で"
                               "いてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_(
                              "Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει απ"
                              "ό την απαίτησή του για άμεσο \"κούρεμα\" του"
                              " ελληνικού χρέους, ώστε να αποκατασταθεί η β"
                              "ιωσιμότητά του, υπό την προϋπόθεση όμως ότι "
                              "η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό "
                              "του ελληνικού προγράμματος με αξιόπιστα "
                              "μέτρα.")));
    EXPECT(RF_SUCCESS,rf_textfile_read_line_chars(&f, &line,87));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_(
                              "河村氏は２０日、東京都内で維新の石原代表と会"
                              "談した。同席者によると、石原氏からはその場で"
                              "、来月４日に公示される衆院選までに維新と合流"
                              "することは困難との見通しが示されたという。")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφ"
                    "ε την Κυριακή ο Carmela Avner, Chief Informa"
                    "tion Officer της ισραηλινής κυβέρνησης. «Το "
                    "πρώτο είναι το πραγματικό πεδίο της μάχης, τ"
                    "ο δεύτερο είναι τα social media και το τρίτο"
                    " είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπ"
                    "αθούν να παρεμποδίσουν την πρόσβαση στις κρα"
                    "τικές ιστοσελίδες σε καθημερινή βάση. Επομέν"
                    "ως, αυτή τη χρονική στιγμή, είναι εξαιρετικά"
                    " σημαντικό να προστατεύσουμε τα ηλεκτρονικά "
                    "μας συστήματα».")));
    EXPECT(RE_FILE_EOF, rf_textfile_read_line(&f, &line));
    rf_textfile_deinit(&f);

	//utf16 be file
    EXPECT(true, rf_textfile_init(&f, RFS_("utf16betextfile"),
                                 RF_FILE_READ,
                                 RF_BIG_ENDIAN, RF_UTF16, RF_EOL_LF));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true, rf_string_equal(&line,
                                RFS_("This is a Big Endian UTF-16 File")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("【男子は必見】女子力MAXのイケてる女子が語る "
                               "/ デートで電車を使う男は女子にとって迷惑だ"
                               "から一生独身でいてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("よかったー！うちの地域はJRがディーゼル機関"
                               "車だから電車じゃないわー！ 【男子は必見】女"
                               "子力MAXのイケてる女子が語る / デートで電車"
                               "を使う男は女子にとって迷惑だから一生独身で"
                               "いてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει απ"
                    "ό την απαίτησή του για άμεσο \"κούρεμα\" του"
                    " ελληνικού χρέους, ώστε να αποκατασταθεί η β"
                    "ιωσιμότητά του, υπό την προϋπόθεση όμως ότι "
                    "η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό "
                    "του ελληνικού προγράμματος με αξιόπιστα "
                    "μέτρα.")));
    EXPECT(RF_SUCCESS,rf_textfile_read_line_chars(&f, &line,87));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("河村氏は２０日、東京都内で維新の石原代表と会"
                    "談した。同席者によると、石原氏からはその場で"
                    "、来月４日に公示される衆院選までに維新と合流"
                    "することは困難との見通しが示されたという。")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφ"
                    "ε την Κυριακή ο Carmela Avner, Chief Informa"
                    "tion Officer της ισραηλινής κυβέρνησης. «Το "
                    "πρώτο είναι το πραγματικό πεδίο της μάχης, τ"
                    "ο δεύτερο είναι τα social media και το τρίτο"
                    " είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπ"
                    "αθούν να παρεμποδίσουν την πρόσβαση στις κρα"
                    "τικές ιστοσελίδες σε καθημερινή βάση. Επομέν"
                    "ως, αυτή τη χρονική στιγμή, είναι εξαιρετικά"
                    " σημαντικό να προστατεύσουμε τα ηλεκτρονικά "
                    "μας συστήματα».")));
    EXPECT(RE_FILE_EOF, rf_textfile_read_line(&f, &line));
    rf_textfile_deinit(&f);

	//utf32 be file
    EXPECT(true,
           rf_textfile_init(&f, RFS_("utf32betextfile"),
                           RF_FILE_READ, RF_BIG_ENDIAN,
                           RF_UTF32, RF_EOL_LF));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           /*it's a big endian file but the string is wrong atm */
           /* TODO: Find an editor and edit the file to correct it */
           rf_string_equal(&line,
                          RFS_("This is a Little Endian UTF-32 File")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("【男子は必見】女子力MAXのイケてる女子が語る "
                               "/ デートで電車を使う男は女子にとって迷惑だ"
                               "から一生独身でいてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(&line,
                          RFS_("よかったー！うちの地域はJRがディーゼル機関"
                               "車だから電車じゃないわー！ 【男子は必見】女"
                               "子力MAXのイケてる女子が語る / デートで電車"
                               "を使う男は女子にとって迷惑だから一生独身で"
                               "いてください")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει απ"
                    "ό την απαίτησή του για άμεσο \"κούρεμα\" του"
                    " ελληνικού χρέους, ώστε να αποκατασταθεί η β"
                    "ιωσιμότητά του, υπό την προϋπόθεση όμως ότι "
                    "η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό "
                    "του ελληνικού προγράμματος με αξιόπιστα "
                    "μέτρα.")));
    EXPECT(RF_SUCCESS,rf_textfile_read_line_chars(&f, &line,87));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_(
                   "河村氏は２０日、東京都内で維新の石原代表と会"
                   "談した。同席者によると、石原氏からはその場で"
                   "、来月４日に公示される衆院選までに維新と合流"
                   "することは困難との見通しが示されたという。")));
    EXPECT(RF_SUCCESS, rf_textfile_read_line(&f, &line));
    EXPECT(true,
           rf_string_equal(
               &line,
               RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφ"
                    "ε την Κυριακή ο Carmela Avner, Chief Informa"
                    "tion Officer της ισραηλινής κυβέρνησης. «Το "
                    "πρώτο είναι το πραγματικό πεδίο της μάχης, τ"
                    "ο δεύτερο είναι τα social media και το τρίτο"
                    " είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπ"
                    "αθούν να παρεμποδίσουν την πρόσβαση στις κρα"
                    "τικές ιστοσελίδες σε καθημερινή βάση. Επομέν"
                    "ως, αυτή τη χρονική στιγμή, είναι εξαιρετικά"
                    " σημαντικό να προστατεύσουμε τα ηλεκτρονικά "
                    "μας συστήματα».")));
    EXPECT(RE_FILE_EOF, rf_textfile_read_line(&f, &line));
    rf_textfile_deinit(&f);;

	rf_stringx_deinit(&line);
	return 0;
}
