#include "Widget.h"
#include "Manager.h"

Widget::Widget() {

}

Widget::Widget(int id) {
	myid=id;
}

Widget::~Widget() {

}

int Widget::getId() {
	return myid;
}

void *Widget::getInstance() {
	return me;
}

void Widget::processEvent(const MAEvent & e) {

}

void Widget::addActionListener(ActionListener *a) {
	actionListener=a;
}

bool Widget::operator == ( const Widget & w ) const {
	if( w.myid == myid ) return true;
	else return false;
}

bool Widget::operator < ( const Widget & w ) const {
	if( w.myid < myid ) return true;
	else return false;
}