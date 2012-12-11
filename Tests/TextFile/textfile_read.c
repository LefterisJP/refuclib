#include <RFtextfile.h>
#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"

int main()
{
	RF_TextFile f;
	RF_StringX line;
	uint32_t length;

	rfInit();
	//initialize the buffer
	EXPECT(true,rfStringX_Init_buff(&line,4096,""))
	
	//utf8 file
	EXPECT(RF_SUCCESS,rfTextFile_Init(&f,RFS_("utf8textfile"),RF_FILE_READ,RF_UTF8,RF_EOL_LF))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("This is a UTF-8 File")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("よかったー！うちの地域はJRがディーゼル機関車だから電車じゃないわー！ 【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει από την απαίτησή του για άμεσο \"κούρεμα\" του ελληνικού χρέους, ώστε να αποκατασταθεί η βιωσιμότητά του, υπό την προϋπόθεση όμως ότι η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό του ελληνικού προγράμματος με αξιόπιστα μέτρα.")))	
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine(&f,&line,87))
	EXPECT(true,rfString_Equal(&line,RFS_("河村氏は２０日、東京都内で維新の石原代表と会談した。同席者によると、石原氏からはその場で、来月４日に公示される衆院選までに維新と合流することは困難との見通しが示されたという。")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφε την Κυριακή ο Carmela Avner, Chief Information Officer της ισραηλινής κυβέρνησης. «Το πρώτο είναι το πραγματικό πεδίο της μάχης, το δεύτερο είναι τα social media και το τρίτο είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπαθούν να παρεμποδίσουν την πρόσβαση στις κρατικές ιστοσελίδες σε καθημερινή βάση. Επομένως, αυτή τη χρονική στιγμή, είναι εξαιρετικά σημαντικό να προστατεύσουμε τα ηλεκτρονικά μας συστήματα».")))	
	EXPECT(RE_FILE_EOF,rfTextFile_ReadLine2(&f,&line))
	rfTextFile_Deinit(&f);
	
	//utf16 le file
	EXPECT(RF_SUCCESS,rfTextFile_Init(&f,RFS_("utf16letextfile"),RF_FILE_READ,RF_UTF16_LE,RF_EOL_LF))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("This is a Little Endian UTF-16 File")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("よかったー！うちの地域はJRがディーゼル機関車だから電車じゃないわー！ 【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει από την απαίτησή του για άμεσο \"κούρεμα\" του ελληνικού χρέους, ώστε να αποκατασταθεί η βιωσιμότητά του, υπό την προϋπόθεση όμως ότι η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό του ελληνικού προγράμματος με αξιόπιστα μέτρα.")))	
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine(&f,&line,87))
	EXPECT(true,rfString_Equal(&line,RFS_("河村氏は２０日、東京都内で維新の石原代表と会談した。同席者によると、石原氏からはその場で、来月４日に公示される衆院選までに維新と合流することは困難との見通しが示されたという。")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφε την Κυριακή ο Carmela Avner, Chief Information Officer της ισραηλινής κυβέρνησης. «Το πρώτο είναι το πραγματικό πεδίο της μάχης, το δεύτερο είναι τα social media και το τρίτο είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπαθούν να παρεμποδίσουν την πρόσβαση στις κρατικές ιστοσελίδες σε καθημερινή βάση. Επομένως, αυτή τη χρονική στιγμή, είναι εξαιρετικά σημαντικό να προστατεύσουμε τα ηλεκτρονικά μας συστήματα».")))	
	EXPECT(RE_FILE_EOF,rfTextFile_ReadLine2(&f,&line))
	rfTextFile_Deinit(&f);
	
	//utf16 be file
	EXPECT(RF_SUCCESS,rfTextFile_Init(&f,RFS_("utf16betextfile"),RF_FILE_READ,RF_UTF16_BE,RF_EOL_LF))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("This is a Big Endian UTF-16 File")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("よかったー！うちの地域はJRがディーゼル機関車だから電車じゃないわー！ 【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει από την απαίτησή του για άμεσο \"κούρεμα\" του ελληνικού χρέους, ώστε να αποκατασταθεί η βιωσιμότητά του, υπό την προϋπόθεση όμως ότι η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό του ελληνικού προγράμματος με αξιόπιστα μέτρα.")))	
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine(&f,&line,87))
	EXPECT(true,rfString_Equal(&line,RFS_("河村氏は２０日、東京都内で維新の石原代表と会談した。同席者によると、石原氏からはその場で、来月４日に公示される衆院選までに維新と合流することは困難との見通しが示されたという。")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφε την Κυριακή ο Carmela Avner, Chief Information Officer της ισραηλινής κυβέρνησης. «Το πρώτο είναι το πραγματικό πεδίο της μάχης, το δεύτερο είναι τα social media και το τρίτο είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπαθούν να παρεμποδίσουν την πρόσβαση στις κρατικές ιστοσελίδες σε καθημερινή βάση. Επομένως, αυτή τη χρονική στιγμή, είναι εξαιρετικά σημαντικό να προστατεύσουμε τα ηλεκτρονικά μας συστήματα».")))	
	EXPECT(RE_FILE_EOF,rfTextFile_ReadLine2(&f,&line))
	rfTextFile_Deinit(&f);
	
	//utf32 le file
	EXPECT(RF_SUCCESS,rfTextFile_Init(&f,RFS_("utf32letextfile"),RF_FILE_READ,RF_UTF32_LE,RF_EOL_LF))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("This is a Little Endian UTF-32 File")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("よかったー！うちの地域はJRがディーゼル機関車だから電車じゃないわー！ 【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει από την απαίτησή του για άμεσο \"κούρεμα\" του ελληνικού χρέους, ώστε να αποκατασταθεί η βιωσιμότητά του, υπό την προϋπόθεση όμως ότι η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό του ελληνικού προγράμματος με αξιόπιστα μέτρα.")))	
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine(&f,&line,87))
	EXPECT(true,rfString_Equal(&line,RFS_("河村氏は２０日、東京都内で維新の石原代表と会談した。同席者によると、石原氏からはその場で、来月４日に公示される衆院選までに維新と合流することは困難との見通しが示されたという。")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφε την Κυριακή ο Carmela Avner, Chief Information Officer της ισραηλινής κυβέρνησης. «Το πρώτο είναι το πραγματικό πεδίο της μάχης, το δεύτερο είναι τα social media και το τρίτο είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπαθούν να παρεμποδίσουν την πρόσβαση στις κρατικές ιστοσελίδες σε καθημερινή βάση. Επομένως, αυτή τη χρονική στιγμή, είναι εξαιρετικά σημαντικό να προστατεύσουμε τα ηλεκτρονικά μας συστήματα».")))	
	EXPECT(RE_FILE_EOF,rfTextFile_ReadLine2(&f,&line))
	rfTextFile_Deinit(&f);
	
	//utf32 be file
	EXPECT(RF_SUCCESS,rfTextFile_Init(&f,RFS_("utf32betextfile"),RF_FILE_READ,RF_UTF32_BE,RF_EOL_LF))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("This is a Big Endian UTF-32 File")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("よかったー！うちの地域はJRがディーゼル機関車だから電車じゃないわー！ 【男子は必見】女子力MAXのイケてる女子が語る / デートで電車を使う男は女子にとって迷惑だから一生独身でいてください")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("Το ΔΝΤ εμφανίζεται έτοιμο να υπαναχωρήσει από την απαίτησή του για άμεσο \"κούρεμα\" του ελληνικού χρέους, ώστε να αποκατασταθεί η βιωσιμότητά του, υπό την προϋπόθεση όμως ότι η Ευρωζώνη θα καλύψει το χρηματοδοτικό κενό του ελληνικού προγράμματος με αξιόπιστα μέτρα.")))	
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine(&f,&line,87))
	EXPECT(true,rfString_Equal(&line,RFS_("河村氏は２０日、東京都内で維新の石原代表と会談した。同席者によると、石原氏からはその場で、来月４日に公示される衆院選までに維新と合流することは困難との見通しが示されたという。")))
	EXPECT(RF_SUCCESS,rfTextFile_ReadLine2(&f,&line))
	EXPECT(true,rfString_Equal(&line,RFS_("«Ο πόλεμος διεξάγεται σε τρία μέτωπα», έγραφε την Κυριακή ο Carmela Avner, Chief Information Officer της ισραηλινής κυβέρνησης. «Το πρώτο είναι το πραγματικό πεδίο της μάχης, το δεύτερο είναι τα social media και το τρίτο είναι οι κυβερνοεπιθέσεις. Οι δράστες προσπαθούν να παρεμποδίσουν την πρόσβαση στις κρατικές ιστοσελίδες σε καθημερινή βάση. Επομένως, αυτή τη χρονική στιγμή, είναι εξαιρετικά σημαντικό να προστατεύσουμε τα ηλεκτρονικά μας συστήματα».")))	
	EXPECT(RE_FILE_EOF,rfTextFile_ReadLine2(&f,&line))
	rfTextFile_Deinit(&f);
	
	rfStringX_Deinit(&line);
	return 0;
}
