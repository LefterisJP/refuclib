
//! [Tokenize_C]

int i;
//Initialize a sentence
RF_String sentence;
unsigned int wordsN;
rfString_Init(&sentence,"This is a full sentence");
//this will be the array that will keep the words
RF_String* words;
//tokenize. Note the '&' operator usage
rfString_Tokenize(&sentence," ",&wordsN,&words);
//print the words one by one
for(i = 0; i < wordsN; i ++)
{
	printf("Words[%d]: %s\n",i,rfString_ToCstr(&words[i]));
}

//when you no longer need them you free them like this
for(i = 0; i < wordsN; i++)
{
	//deinit each word
	rfString_Deinit(&words[i]);
}
//finally free the array
free(words);

//! [Tokenize_C]

//! [Tokenize_CPP]
RF::String s("This is a full sentence");
//the array to hold our string pointer
RF::String** words;
//will hold the number of words
unsigned int wordsN;
//tokenize. Note the usage of the '&' operator
s.Tokenize(" ",&wordsN,&words);
//print each word
for(int i = 0; i < wordsN; i ++)
{
	printf("Words[%d]: %s\n",i,words[i]->ToCstr());
}

//when you no longer need them free them like this
for(int i = 0; i < wordsN; i++)
{
	delete words[i];
}
//finally free the array
free(words);
//! [Tokenize_CPP]
