// msg.h

#define TEXT_LEN 80

struct AMessage
{
	 long msgType_;
	 float floatPt_;
	 int integer_;
	 char text_[TEXT_LEN];
};