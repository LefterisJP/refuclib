#include <RFtextfile.h>
#include <RFstring.h>
#include <refu.h>

#include "../refu_tests.h"

//the lines against which we check each file type
static struct RFstring lines[5];

void writeTest(int endianess, int encoding, struct RFstringx* buff)
{
    struct RFtextfile outF, inF;
    uint32_t i=0; 

    EXPECT(true,
           rf_textfile_init(&outF,RFS_("writefile"),
                           RF_FILE_NEW, endianess, encoding, RF_EOL_LF));
    EXPECT(true,
           rf_textfile_write(
               &outF,
               RFS_("फ्रांस की एक अदालत ने पूर्व राष्ट्रपति निकोला सार्कोज़ी से ग़ैर-क़"
                    "ानूनी राजनीतिक चंदे के आरोप को लेकर बारह घंटे लंबी पूछताछ की है"
                    " और कहा है कि वो संदेह के दायरे में हैं.\n")));
    EXPECT(true,
           rf_textfile_write(
               &outF,
               RFS_("Οργή στους συγγενείς των θυμάτων προκάλεσε η αθωωτική "
                    "απόφαση του Κακουργιοδικείου στους υπεύθυνους της "
                    "αεροπορικής εταιρείας «Ήλιος», αεροσκάφος της οποίας "
                    "συνετρίβη στις 14 Αυγούστου του 2005 στο Γραμματικό "
                    "Αττικής, με 121 θύματα.\n")));
    EXPECT(true,
           rf_textfile_write(
               &outF,
               RFS_("今回初めて「児童生徒の生命・身体の安全がおびやかされ"
                    "るような重大事案」について詳細な報告を求めたところ、"
                    "全国で２７８件あった\n")));

    EXPECT(true,
           rf_textfile_insert(&outF,1,
                             RFS_("We are inserting a line before the first"
                                  " line of the file, replacing the "
                                  "previous first"), false));
    EXPECT(true,
           rf_textfile_insert(
               &outF,3,
               RFS_("We are inserting a line after the third line"), true));
    rf_textfile_deinit(&outF);
	
    //checking if the file was correctly written
    EXPECT(true,
           rf_textfile_init(
               &inF, RFS_("writefile"), RF_FILE_READ,
               endianess, encoding, RF_EOL_LF));
    while(rf_textfile_read_line(&inF,buff) == RF_SUCCESS)
    {
        EXPECT(true,rf_string_equal(buff,&lines[i]));
        i++;
    }
    EXPECT(5,i);
    rf_textfile_deinit(&inF);
}

int main()
{
    struct RFstringx buffer;
	
    DEFAULT_LIB_INIT();
	//initialize the lines we are gonna use for checking
    EXPECT(true,
           rf_string_init(
               &lines[0],
               "We are inserting a line before the first line of the file,"
               " replacing the previous first"));
    EXPECT(true,
           rf_string_init(
               &lines[1],
               "फ्रांस की एक अदालत ने पूर्व राष्ट्रपति निकोला सार्कोज़ी से ग़ैर-क़ानूनी र"
               "ाजनीतिक चंदे के आरोप को लेकर बारह घंटे लंबी पूछताछ की है और कहा है क"
               "ि वो संदेह के दायरे में हैं."));
    EXPECT(true,
           rf_string_init(
               &lines[2],
               "Οργή στους συγγενείς των θυμάτων προκάλεσε η αθωωτική "
               "απόφαση του Κακουργιοδικείου στους υπεύθυνους της "
               "αεροπορικής εταιρείας «Ήλιος», αεροσκάφος της οποίας "
               "συνετρίβη στις 14 Αυγούστου του 2005 στο Γραμματικό "
               "Αττικής, με 121 θύματα."));
    EXPECT(true,
           rf_string_init(
               &lines[3],
               "We are inserting a line after the third line"));
    EXPECT(true,
           rf_string_init(
               &lines[4],
               "今回初めて「児童生徒の生命・身体の安全がおびやかされるよ"
               "うな重大事案」について詳細な報告を求めたところ、全国で２"
               "７８件あった"));
  	//initialize the buffer
    EXPECT(true,rf_stringx_init_buff(&buffer,4096,""));
	
    //test each file type
    writeTest(RF_ENDIANESS_UNKNOWN, RF_UTF8, &buffer);
    writeTest(RF_LITTLE_ENDIAN, RF_UTF16, &buffer);
    writeTest(RF_BIG_ENDIAN, RF_UTF16, &buffer);
    writeTest(RF_LITTLE_ENDIAN, RF_UTF32, &buffer);
    writeTest(RF_BIG_ENDIAN, RF_UTF32, &buffer);
	
    //cleanup
    rf_stringx_deinit(&buffer);
    EXPECT(true, rfDeleteFile(RFS_("writefile")));

    return 0;
}
