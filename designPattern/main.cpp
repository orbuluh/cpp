#include "creational/factory/factoryUseCase.h"
#include "creational/abstractfactory/abstractfactoryUseCase.h"
//#include "creational/prototype/prototypeUseCase.h" // example showned in composite::demo()
#include "structural/adapter/adapterUseCase.h"
#include "structural/bridge/bridgeUseCase.h"
#include "structural/composite/compositeUseCase.h"
#include "structural/decorator/decoratorUseCase.h"
#include "structural/facade/facadeUseCase.h"
#include "structural/flyweight/flyweightUseCase.h"
#include "structural/proxy/proxyUseCase.h"
#include "behavior/chainofresp/chainofrespUseCase.h"
#include "behavior/command/commandUseCase.h"
#include "behavior/iterator/iteratorUseCase.h"
#include "behavior/mediator/mediatorUseCase.h"
#include "behavior/memento/mementoUseCase.h"
#include "behavior/observer/observerUseCase.h"
#include "behavior/state/stateUseCase.h"
#include "behavior/strategy/strategyUseCase.h"
#include "behavior/templatemethod/templatemethodUseCase.h"
#include "behavior/visitor/visitorUseCase.h"
#include "behavior/maybeMonad/maybeMonadUseCase.h"

int main(int argc, char* argv[]) {
    //maybemonad::demo();
    //observer::demo();
    //state::demo();
    //bridge::demo();
    //adapter::demo();
    //facade::demo();
    //mediator::demo();
    //command::demo();
    //chainofresp::demo();
    //decorator::demo();
    //proxy::demo();
    //flyweight::demo();
    //memento::demo();
    //composite::demo(); // also show how prototype is used
    //iterator::demo();
    //visitor::demo();
    //strategy::demo();
    //templatemethod::demo();
    factory::demo();
    abstractfactory::demo();
    return 0;
}
