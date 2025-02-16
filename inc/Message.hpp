#ifndef MESSAGE_HPP
# define MESSAGE_HPP

// Welcome messages
#define RPL_WELCOME             001
#define RPL_YOURHOST           002
#define RPL_CREATED            003
#define RPL_MYINFO             004

// Standard replies
#define RPL_LISTSTART          321
#define RPL_LIST               322
#define RPL_LISTEND            323
#define RPL_TOPIC              332
#define RPL_NOTOPIC            331
#define RPL_NAMREPLY          353
#define RPL_ENDOFNAMES        366
#define RPL_MOTD              372
#define RPL_MOTDSTART         375
#define RPL_ENDOFMOTD         376
#define RPL_INVITING          341

// Custom replies (for internal use - using 900+ range to avoid conflicts)
#define RPL_JOINCHANNEL       901
#define RPL_PART              902
#define RPL_MODE              903
#define RPL_QUIT              904
#define RPL_NICK              905
#define RPL_PING              906
#define RPL_KICK              907
#define RPL_HELPTXT           908
#define RPL_INVITE            909

// Error replies
#define ERR_NOSUCHNICK        401
#define ERR_NOSUCHCHANNEL     403
#define ERR_CANNOTSENDTOCHAN  404
#define ERR_NORECIPIENT       411
#define ERR_NOTEXTTOSEND       412
#define ERR_MSGTOOLONG        414
#define ERR_UNKNOWNCOMMAND    421
#define ERR_NONICKNAMEGIVEN    431
#define ERR_ERRONEUSNICKNAME   432
#define ERR_NICKNAMEINUSE      433
#define ERR_USERNOTINCHANNEL   441
#define ERR_NOTONCHANNEL       442
#define ERR_USERONCHANNEL      443
#define ERR_NEEDMOREPARAMS      461
#define ERR_ALREADYREGISTERED   462
#define ERR_PASSWDMISMATCH      464
#define ERR_SERVICESFULL        471
#define ERR_CHANNELISFULL       471
#define ERR_UNKNOWNMODE         472
#define ERR_INVITEONLYCHAN       473
#define ERR_BANNEDFROMCHAN       474
#define ERR_BADCHANNELKEY        475
#define ERR_CHANOPRIVSNEEDED      482

# endif