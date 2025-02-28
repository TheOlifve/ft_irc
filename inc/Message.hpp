#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#define RPL_WELCOME				1
#define RPL_JOINCHANNEL			2
#define RPL_PART				3
#define RPL_MODE				4
#define RPL_QUIT				5
#define RPL_NICK				6
#define RPL_PING				7
#define RPL_PRIVMSG				8
#define RPL_LISTSTART			321
#define RPL_LIST				322
#define RPL_LISTEND				323
#define RPL_TOPIC				332
#define RPL_NOTOPIC				331
#define RPL_NAMREPLY			353
#define RPL_ENDOFNAMES			366
#define RPL_HELPTXT				704
#define RPL_ENDOFHELP			705
#define ERR_NOSUCHNICK			401
#define ERR_NOSUCHCHANNEL		403
#define ERR_NORECIPIENT			411
#define ERR_NOTEXTTOSEND		412
#define ERR_MSGTOOLONG			414
#define ERR_TOOMANYTARGETS		417
#define ERR_UNKNOWNCOMMAND		421
#define ERR_ERRONEUSNICKNAME	432
#define ERR_NICKNAMEINUSE		433
#define ERR_ERRONEUSUSERNAME	434
#define ERR_USERNOTINCHANNEL	441
#define ERR_NOTONCHANNEL		442
#define ERR_USERONCHANNEL		443
#define ERR_NEEDMOREPARAMS		461
#define ERR_PASSWDMISMATCH		464
#define ERR_SERVICESFULL		471
#define ERR_BADCHANNELKEY		475
#define RPL_CHANNELMODEOP		730
#define RPL_INVITEONLY			731
#define RPL_INVITEFREE			732
#define RPL_TOPICOPONLY			733	// Only operators can set the channel's topic
#define RPL_TOPICOPANYONE		734	// Anyone can set the channel's topic
#define RPL_CHANNELPASS			735	// Custom numeric reply for setting password
#define RPL_CHANNELPASSLESS		736	// Custom numeric reply for making the channel passwordless
#define RPL_ASSIGNOP			737	// Custom numeric reply for setting user as operator
#define ERR_NOTOPERATOR			738	// Error code for user not being an operator in the channel
#define RPL_REMOVEOPERATOR		739	// Response code for user removed from the operator list
#define RPL_REMOVEDOP			740 // Response code for user removed from the operator list
#define RPL_USERLIMITSET		741	// Defining the error code for setting a user limit
#define RPL_USERLIMITREMOVED	742	// Defining the error code for removing the user limit
#define ERR_WRONGMODE			743	// Error code for wrong mode
#define ERR_NOPERMISSION		744	// Error code for lack of permission to change mode
#define ERR_MODEPARAMS			745	// Error code for wrong mode parameters
#define ERR_KEYPARAMS			746	// Error code for wrong key parameters
#define ERR_OPPARAMS			747	// Error code for wrong operator parameters
#define ERR_USERLIMITPARAMS		748	// Error code for wrong user limit parameters
#define ERR_TOPICPARAMS			749 // Custom error code for wrong number of parameters in TOPIC command
#define RPL_TOPICSET			750 // Custom reply code for successful topic set
#define RPL_USERTOPICSET		751 // Custom reply code for successfully setting the channel topic

# endif