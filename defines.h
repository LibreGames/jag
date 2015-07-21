#ifndef DEFINES_H
#define DEFINES_H

#include <QString>

const quint32 NoPlace         = 0;
const quint32 PlaceConcrete   = ((quint32)1) << 0;

const quint32 Target1         = ((quint32)1) << 1;
const quint32 Target2         = ((quint32)1) << 2;

const quint32 Targets         = Target1 | Target2;

const quint32 Block1          = ((quint32)1) << 3;
const quint32 Block2          = ((quint32)1) << 4;

const quint32 Blocks          = Block1 | Block2;

const quint32 ItemStart       = ((quint32)1) << 5;
const quint32 ItemAppear      = ((quint32)1) << 6;

const quint32 Processed       = ((quint32)1) << 15;


const QString GameName        = "JAG";

const int MAX_ITEMS_COUNT     = 6;

#endif // DEFINES_H
