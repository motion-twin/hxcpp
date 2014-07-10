#include <hxcpp.h>
#include "Cppia.h"

namespace hx
{

enum ArrayFunc
{
   afConcat,
   afCopy,
   afInsert,
   afIterator,
   afJoin,
   afPop,
   afPush,
   afRemove,
   afReverse,
   afShift,
   afSlice,
   afSplice,
   afSort,
   afToString,
   afUnshift,
   afMap,
   afFilter,
   afIndexOf,
   afLastIndexOf,
   af__get,
   af__set,
   af__crement,
};

static int sArgCount[] = 
{
   1, //afConcat,
   0, //afCopy,
   2, //afInsert,
   0, //afIterator,
   1, //afJoin,
   0, //afPop,
   1, //afPush,
   1, //afRemove,
   0, //afReverse,
   0, //afShift,
   2, //afSlice,
   2, //afSplice,
   1, //afSort,
   0, //afToString,
   1, //afUnshift,
   1, //afMap,
   1, //afFilter,
   2, //afIndexOf,
   2, //afLastIndexOf,
   1, //af__get,
   2, //af__set,
   1, //af__crement,
};

struct ArrayBuiltinBase : public CppiaExpr
{
   CppiaExpr *thisExpr;
   Expressions args;

   ArrayBuiltinBase(CppiaExpr *inSrc, CppiaExpr *inThisExpr, Expressions &ioExpressions)
      : CppiaExpr(inSrc)
   {
      thisExpr = inThisExpr;
      args.swap(ioExpressions);
   }
   const char *getName() { return "ArrayBuiltinBase"; }


   CppiaExpr *link(CppiaData &inData)
   {
      thisExpr = thisExpr->link(inData);
      for(int a=0;a<args.size();a++)
         args[a] = args[a]->link(inData);
      return this;
   }
};

struct ArrayEq
{
   template<typename ELEM>
   inline static void set(ELEM &elem, CppiaCtx *ctx, CppiaExpr *expr)
   {
      elem = runValue(elem,ctx,expr);
   }
};


struct ArrayAddEq
{
   template<typename ELEM>
   inline static void set(ELEM &elem, CppiaCtx *ctx, CppiaExpr *expr)
   {
      elem = runValue(elem,ctx,expr);
   }
};



template<typename ELEM, typename FUNC>
struct ArraySetter : public ArrayBuiltinBase
{
   ArraySetter(CppiaExpr *inSrc, CppiaExpr *inThisExpr, Expressions &ioExpressions)
      : ArrayBuiltinBase(inSrc,inThisExpr,ioExpressions)
   {
   }
   virtual ExprType getType()
   {
      return (ExprType)ExprTypeOf<ELEM>::value;
   }
   int runInt(CppiaCtx *ctx)
   {
      Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
      int i = args[0]->runInt(ctx);
      ELEM &elem = thisVal->Item(i);
      FUNC::run(elem, ctx, args[1]);
      return ValToInt(elem);
   }
   Float  runFloat(CppiaCtx *ctx)
   {
      Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
      int i = args[0]->runInt(ctx);
      ELEM &elem = thisVal->Item(i);
      FUNC::run(elem, ctx, args[1]);
      return ValToFloat(elem);
   }
   String  runString(CppiaCtx *ctx)
   {
      Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
      int i = args[0]->runInt(ctx);
      ELEM &elem = thisVal->Item(i);
      FUNC::run(elem, ctx, args[1]);
      return ValToString(elem);
   }
   hx::Object *runObject(CppiaCtx *ctx)
   {
      Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
      int i = args[0]->runInt(ctx);
      ELEM &elem = thisVal->Item(i);
      FUNC::run(elem, ctx, args[1]);
      return Dynamic(elem).mPtr;
   }
};



template<typename ELEM, int FUNC, typename CREMENT>
struct ArrayBuiltin : public ArrayBuiltinBase
{
   ArrayBuiltin(CppiaExpr *inSrc, CppiaExpr *inThisExpr, Expressions &ioExpressions)
      : ArrayBuiltinBase(inSrc,inThisExpr,ioExpressions)
   {
   }

   ExprType getType()
   {
      switch(FUNC)
      {
         case af__get:
         case af__set:
         case af__crement:
            return (ExprType)ExprTypeOf<ELEM>::value;

         case afPop:
         //case afUnshift:
            if (ExprTypeOf<ELEM>::value==etString)
               return etString;
            return etObject;

         case afJoin:
         case afToString:
            return etString;

         case afSort:
         case afInsert:
         case afUnshift:
            return etVoid;

         case afPush:
         case afRemove:
         case afIndexOf:
         case afLastIndexOf:
            return etInt;

         default:
            return etObject;
      }

      return etObject;
   }


   int runInt(CppiaCtx *ctx)
   {
      if (FUNC==afPush)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM elem;
         int result =  thisVal->push( runValue(elem,ctx,args[0]) );
         return result;
      }
      if (FUNC==afPop)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToInt(thisVal->pop());
      }
      if (FUNC==afShift)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToInt(thisVal->shift());
      }
      if (FUNC==afRemove)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM elem;
         return thisVal->remove(runValue(elem,ctx,args[0]));
      }
      if (FUNC==afIndexOf)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM elem;
         runValue(elem,ctx,args[0]);
         return thisVal->indexOf(elem, args[1]->runObject(ctx));
      }
      if (FUNC==afLastIndexOf)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM elem;
         runValue(elem,ctx,args[0]);
         return thisVal->lastIndexOf(elem, args[1]->runObject(ctx));
      }
      if (FUNC==af__get)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToInt(thisVal->__get(args[0]->runInt(ctx)));
      }
      if (FUNC==af__set)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int i = args[0]->runInt(ctx);
         ELEM elem;
         thisVal->Item(i) = runValue(elem,ctx,args[1]);
         return ValToInt(elem);
      }
      if (FUNC==af__crement)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM &elem = thisVal->Item(args[0]->runInt(ctx));
         return ValToInt(CREMENT::run(elem));
      }




      return 0;
   }
   Float       runFloat(CppiaCtx *ctx)
   {
      if (FUNC==afPop)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToFloat(thisVal->pop());
      }
      if (FUNC==afShift)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToFloat(thisVal->shift());
      }
      if (FUNC==af__get)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToFloat(thisVal->__get(args[0]->runInt(ctx)));
      }
      if (FUNC==af__set)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int i = args[0]->runInt(ctx);
         ELEM elem;
         thisVal->Item(i) = runValue(elem,ctx,args[1]);
         return ValToFloat(elem);
      }
      if (FUNC==af__crement)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM &elem = thisVal->Item(args[0]->runInt(ctx));
         return ValToFloat(CREMENT::run(elem));
      }




      if (FUNC==afPush)
         return runInt(ctx);
      return 0.0;
   }


   ::String    runString(CppiaCtx *ctx)
   {
      if (FUNC==afPop)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToString(thisVal->pop());
      }
      if (FUNC==afShift)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToString(thisVal->shift());
      }
      if (FUNC==af__get)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return ValToString(thisVal->__get(args[0]->runInt(ctx)));
      }
      if (FUNC==af__set)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int i = args[0]->runInt(ctx);
         ELEM elem;
         thisVal->Item(i) = runValue(elem,ctx,args[1]);
         return ValToString(elem);
      }
      if (FUNC==af__crement)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM &elem = thisVal->Item(args[0]->runInt(ctx));
         return ValToString(CREMENT::run(elem));
      }


      if (FUNC==afJoin)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return thisVal->join( args[0]->runString(ctx) );
      }

      if (FUNC==afToString)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return thisVal->toString();
      }

      if (FUNC==afPush)
         return Dynamic(runInt(ctx))->toString();

      return runObject(ctx)->toString();
   }
   hx::Object *runObject(CppiaCtx *ctx)
   {
      if (FUNC==af__get)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return thisVal->__GetItem(args[0]->runInt(ctx)).mPtr;
      }
      if (FUNC==af__set)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int i = args[0]->runInt(ctx);
         ELEM elem;
         thisVal->Item(i) = runValue(elem,ctx,args[1]);
         return Dynamic(elem).mPtr;
      }
      if (FUNC==af__crement)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM &elem = thisVal->Item(args[0]->runInt(ctx));
         return Dynamic(CREMENT::run(elem)).mPtr;
      }

      if (FUNC==afPop)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return Dynamic(thisVal->pop()).mPtr;
      }
      if (FUNC==afShift)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return Dynamic(thisVal->shift()).mPtr;
      }


      if (FUNC==afConcat)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         Array_obj<ELEM> *inVal = (Array_obj<ELEM>*)args[0]->runObject(ctx);
         return thisVal->concat(inVal).mPtr;
      }
      if (FUNC==afCopy)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return thisVal->copy().mPtr;
      }
      if (FUNC==afSplice)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int pos = args[0]->runInt(ctx);
         int end = args[1]->runInt(ctx);
         return thisVal->splice(pos,end).mPtr;
      }
      if (FUNC==afSlice)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int pos = args[0]->runInt(ctx);
         hx::Object *end = args[1]->runObject(ctx);
         return thisVal->slice(pos,end).mPtr;
      }
      if (FUNC==afMap)
      {
         // TODO - maybe make this more efficient
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         Array<ELEM> result = thisVal->map(args[0]->runObject(ctx));
         return result.mPtr;
      }
      if (FUNC==afFilter)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return thisVal->filter(args[0]->runObject(ctx)).mPtr;
      }

      if (FUNC==afIterator)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         return thisVal->iterator().mPtr;
      }

      if (FUNC==afPush)
         return Dynamic(runInt(ctx)).mPtr;

      if (FUNC==afRemove)
         return Dynamic((bool)runInt(ctx)).mPtr;

      if (FUNC==afJoin)
         return Dynamic(runString(ctx)).mPtr;


      return 0;
   }
   void        runVoid(CppiaCtx *ctx)
   {
      if (FUNC==afPop)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         thisVal->pop();
      }
      if (FUNC==afShift)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         thisVal->shift();
      }
      if (FUNC==af__set)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int i = args[0]->runInt(ctx);
         ELEM elem;
         thisVal->Item(i) = runValue(elem,ctx,args[1]);
      }
      if (FUNC==af__crement)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM &elem = thisVal->Item(args[0]->runInt(ctx));
         CREMENT::run(elem);
      }


      if (FUNC==afPush || FUNC==afRemove)
         runInt(ctx);
      if (FUNC==afConcat || FUNC==afCopy || FUNC==afReverse || FUNC==afSplice || FUNC==afSlice ||
           FUNC==afMap || FUNC==afFilter)
         runObject(ctx);
      if (FUNC==afReverse)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         thisVal->reverse();
      }
      if (FUNC==afSort)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         thisVal->sort(args[0]->runObject(ctx));
      }
      if (FUNC==afInsert)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         int pos = args[0]->runInt(ctx);
         ELEM elem;
         thisVal->insert(pos,runValue(elem,ctx,args[1]));
      }
      if (FUNC==afUnshift)
      {
         Array_obj<ELEM> *thisVal = (Array_obj<ELEM>*)thisExpr->runObject(ctx);
         ELEM elem;
         thisVal->unshift(runValue(elem,ctx,args[0]));
      }


   }

   CppiaExpr   *makeSetter(AssignOp op,CppiaExpr *inValue)
   {
      if (FUNC==af__get)
      {
         args.push_back(inValue);
         CppiaExpr *replace = 0;

         switch(op)
         {
            case aoSet:
               replace = new ArraySetter<ELEM,AssignSet>(this,thisExpr,args);
               break;
            case aoAdd:
               replace = new ArraySetter<ELEM,AssignAdd>(this,thisExpr,args);
               break;
            case aoMult:
               replace = new ArraySetter<ELEM,AssignMult>(this,thisExpr,args);
               break;
            case aoDiv:
               replace = new ArraySetter<ELEM,AssignDiv>(this,thisExpr,args);
               break;
            case aoSub:
               replace = new ArraySetter<ELEM,AssignSub>(this,thisExpr,args);
               break;
            case aoAnd:
               replace = new ArraySetter<ELEM,AssignAnd>(this,thisExpr,args);
               break;
            case aoOr:
               replace = new ArraySetter<ELEM,AssignOr>(this,thisExpr,args);
               break;
            case aoXOr:
               replace = new ArraySetter<ELEM,AssignXOr>(this,thisExpr,args);
               break;
            case aoShl:
               replace = new ArraySetter<ELEM,AssignShl>(this,thisExpr,args);
               break;
            case aoShr:
               replace = new ArraySetter<ELEM,AssignShr>(this,thisExpr,args);
               break;
            case aoUShr:
               replace = new ArraySetter<ELEM,AssignUShr>(this,thisExpr,args);
               break;
            case aoMod:
               replace = new ArraySetter<ELEM,AssignMod>(this,thisExpr,args);
               break;
      
            default: ;
               printf("make setter %d\n", op);
               throw "setter not implemented";
         }

         delete this;
         return replace;
      }
      return 0;
   }

   CppiaExpr   *makeCrement(CrementOp inOp)
   {
      if (FUNC==af__get)
      {
         CppiaExpr *replace = 0;

         switch(inOp)
         {
            case coPreInc :
               replace = new ArrayBuiltin<ELEM,af__crement,CrementPreInc>(this,thisExpr,args);
               break;
            case coPostInc :
               replace = new ArrayBuiltin<ELEM,af__crement,CrementPostInc>(this,thisExpr,args);
               break;
            case coPreDec :
               replace = new ArrayBuiltin<ELEM,af__crement,CrementPreDec>(this,thisExpr,args);
               break;
            case coPostDec :
               replace = new ArrayBuiltin<ELEM,af__crement,CrementPostDec>(this,thisExpr,args);
               break;
            default:
               return 0;
         }
         return replace;
      }
      return 0;
   }

};




template<int FUNC,int OP>
struct ArrayBuiltinAny : public ArrayBuiltinBase
{
   ArrayBuiltinAny(CppiaExpr *inSrc, CppiaExpr *inThisExpr, Expressions &ioExpressions)
      : ArrayBuiltinBase(inSrc,inThisExpr,ioExpressions)
   {
   }

   int  runInt(CppiaCtx *ctx)
   {
      if (FUNC==afPush)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         return thisVal->__push( args[0]->runObject(ctx) );
      }
      if (FUNC==afRemove)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         return thisVal->__remove( args[0]->runObject(ctx) );
      }
      if (FUNC==afIndexOf)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         Dynamic a0 = args[0]->runObject(ctx);
         return thisVal->__indexOf( a0, args[1]->runObject(ctx) );
      }
      if (FUNC==afLastIndexOf)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         Dynamic a0 = args[0]->runObject(ctx);
         return thisVal->__lastIndexOf( a0, args[1]->runObject(ctx) );
      }

      return Dynamic( runObject(ctx) );
   }

   Float runFloat(CppiaCtx *ctx)
   {
      return runInt(ctx);
   }


   ::String    runString(CppiaCtx *ctx)
   {
      if (FUNC==afJoin)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         return thisVal->__join( args[0]->runObject(ctx) );
      }

      if (FUNC==afToString)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         return thisVal->toString();
      }

      return runObject(ctx)->toString();
   }

   hx::Object *runObject(CppiaCtx *ctx)
   {
      if (FUNC==afPush || FUNC==afRemove || FUNC==afIndexOf || FUNC==afLastIndexOf)
         return Dynamic(runInt(ctx)).mPtr;

      if (FUNC==afRemove)
         return Dynamic((bool)runInt(ctx)).mPtr;

      if (FUNC==afJoin || FUNC==afToString)
         return Dynamic(runString(ctx)).mPtr;

      if (FUNC==afSort || FUNC==afInsert || FUNC==afUnshift)
      {
         runVoid(ctx);
         return 0;
      }


      ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
      CPPIA_CHECK(thisVal);

      if (FUNC==af__get)
      {
         return thisVal->__GetItem(args[0]->runInt(ctx)).mPtr;
      }
      if (FUNC==af__set)
      {
         int i = args[0]->runInt(ctx);
         Dynamic val = args[1]->runObject(ctx);
         if (OP==aoSet)
            return thisVal->__SetItem(i, val).mPtr;

         Dynamic orig = thisVal->__GetItem(i);
         CPPIA_CHECK(orig);
         switch(OP)
         {
            case aoSet: break;

            case aoAdd:
               val = orig + val;
               break;
            case aoMult:
               val = orig * val;
               break;
            case aoDiv:
               val = orig / val;
               break;
            case aoSub:
               val = orig - val;
               break;
            case aoAnd:
               val = orig->__ToInt() & val->__ToInt();
               break;
            case aoOr:
               val = orig->__ToInt() | val->__ToInt();
               break;
            case aoXOr:
               val = orig->__ToInt() ^ val->__ToInt();
               break;
            case aoShl:
               val = orig->__ToInt() << val->__ToInt();
               break;
            case aoShr:
               val = orig->__ToInt() >> val->__ToInt();
               break;
            case aoUShr:
               val = hx::UShr(orig,val);
               break;
            case aoMod:
               val = orig % val;
               break;
            default: ;
         }
         return thisVal->__SetItem(i,val).mPtr;
      }
      if (FUNC==af__crement)
      {
         int i = args[0]->runInt(ctx);
         if (OP==coPreInc)
            return thisVal->__SetItem(i, thisVal->__GetItem(i) + 1).mPtr;
         if (OP==coPreDec)
            return thisVal->__SetItem(i, thisVal->__GetItem(i) - 1).mPtr;

         Dynamic result = thisVal->__GetItem(i);
         if (OP==coPostDec)
            thisVal->__SetItem(i, thisVal->__GetItem(i) - 1);
         if (OP==coPostInc)
            thisVal->__SetItem(i, thisVal->__GetItem(i) + 1);
         return result.mPtr;
      }

      if (FUNC==afPop)
      {
         return thisVal->__pop().mPtr;
      }
      if (FUNC==afShift)
      {
         return thisVal->__shift().mPtr;
      }

      if (FUNC==afConcat)
      {
         return thisVal->__concat(args[0]->runObject(ctx)).mPtr;
      }
      if (FUNC==afCopy)
      {
         return thisVal->__copy().mPtr;
      }
      if (FUNC==afSplice)
      {
         Dynamic pos = args[0]->runObject(ctx);
         Dynamic end = args[1]->runObject(ctx);
         return thisVal->__splice(pos,end).mPtr;
      }
      if (FUNC==afSlice)
      {
         Dynamic pos = args[0]->runObject(ctx);
         Dynamic end = args[1]->runObject(ctx);
         return thisVal->__slice(pos,end).mPtr;
      }
      if (FUNC==afMap)
      {
         return thisVal->__map(args[0]->runObject(ctx)).mPtr;
      }
      if (FUNC==afFilter)
      {
         return thisVal->__filter(args[0]->runObject(ctx)).mPtr;
      }

      if (FUNC==afIterator)
      {
         return thisVal->__iterator().mPtr;
      }

      return 0;
   }
   void runVoid(CppiaCtx *ctx)
   {
      if (FUNC==afSort)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         thisVal->__sort(args[0]->runObject(ctx));
         return;
      }
      if (FUNC==afInsert)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         Dynamic pos = args[0]->runObject(ctx);
         thisVal->__insert(pos, args[1]->runObject(ctx));
         return;
      }
      if (FUNC==afUnshift)
      {
         ArrayBase *thisVal = (ArrayBase *)thisExpr->runObject(ctx);
         thisVal->__unshift(args[0]->runObject(ctx));
         return;
      }

      switch(inlineGetType())
      {
         case etString:
            runString(ctx);
            return;
         case etInt:
            runInt(ctx);
            return;
         default:
            runObject(ctx);
      }
   }

   CppiaExpr   *makeSetter(AssignOp op,CppiaExpr *inValue)
   {
      if (FUNC==af__get)
      {
         args.push_back(inValue);
         CppiaExpr *replace = 0;

         switch(op)
         {
            case aoSet:
               replace = new ArrayBuiltinAny<af__set,aoSet>(this,thisExpr,args);
               break;
            case aoAdd:
               replace = new ArrayBuiltinAny<af__set,aoAdd>(this,thisExpr,args);
               break;
            case aoMult:
               replace = new ArrayBuiltinAny<af__set,aoMult>(this,thisExpr,args);
               break;
            case aoDiv:
               replace = new ArrayBuiltinAny<af__set,aoDiv>(this,thisExpr,args);
               break;
            case aoSub:
               replace = new ArrayBuiltinAny<af__set,aoSub>(this,thisExpr,args);
               break;
            case aoAnd:
               replace = new ArrayBuiltinAny<af__set,aoAnd>(this,thisExpr,args);
               break;
            case aoOr:
               replace = new ArrayBuiltinAny<af__set,aoOr>(this,thisExpr,args);
               break;
            case aoXOr:
               replace = new ArrayBuiltinAny<af__set,aoXOr>(this,thisExpr,args);
               break;
            case aoShl:
               replace = new ArrayBuiltinAny<af__set,aoShl>(this,thisExpr,args);
               break;
            case aoShr:
               replace = new ArrayBuiltinAny<af__set,aoShr>(this,thisExpr,args);
               break;
            case aoUShr:
               replace = new ArrayBuiltinAny<af__set,aoUShr>(this,thisExpr,args);
               break;
            case aoMod:
               replace = new ArrayBuiltinAny<af__set,aoMod>(this,thisExpr,args);
               break;
      
            default: ;
               printf("make setter %d\n", op);
               throw "setter not implemented";
         }

         delete this;
         return replace;
      }
      return 0;
   }

   CppiaExpr   *makeCrement(CrementOp inOp)
   {
      if (FUNC==af__get)
      {
         CppiaExpr *replace = 0;

         switch(inOp)
         {
            case coPreInc :
               replace = new ArrayBuiltinAny<af__crement,coPreInc>(this,thisExpr,args);
               break;
            case coPostInc :
               replace = new ArrayBuiltinAny<af__crement,coPostInc>(this,thisExpr,args);
               break;
            case coPreDec :
               replace = new ArrayBuiltinAny<af__crement,coPreDec>(this,thisExpr,args);
               break;
            case coPostDec :
               replace = new ArrayBuiltinAny<af__crement,coPostDec>(this,thisExpr,args);
               break;
            default:
               return 0;
         }
         return replace;
      }
      return 0;
   }


   inline ExprType inlineGetType()
   {
      switch(FUNC)
      {
         case afJoin:
         case afToString:
            return etString;

         case afSort:
         case afInsert:
         case afUnshift:
            return etVoid;

         case afPush:
         case afRemove:
         case afIndexOf:
         case afLastIndexOf:
            return etInt;

         default:
            return etObject;
      }

      return etObject;
   }
   ExprType getType() { return inlineGetType(); }




};




template<int BUILTIN,typename CREMENT>
CppiaExpr *TCreateArrayBuiltin(CppiaExpr *inSrc, ArrayType inType, CppiaExpr *thisExpr, Expressions &args)
{
   if (sArgCount[BUILTIN]!=args.size())
      throw "Bad arg count for array builtin";

   switch(inType)
   {
      case arrBool:
         return new ArrayBuiltin<bool,BUILTIN,CREMENT>(inSrc, thisExpr, args);
      case arrUnsignedChar:
         return new ArrayBuiltin<unsigned char,BUILTIN,CREMENT>(inSrc, thisExpr, args);
      case arrInt:
         return new ArrayBuiltin<int,BUILTIN,CREMENT>(inSrc, thisExpr, args);
      case arrFloat:
         return new ArrayBuiltin<Float,BUILTIN,CREMENT>(inSrc, thisExpr, args);
      case arrString:
         return new ArrayBuiltin<String,BUILTIN,CREMENT>(inSrc, thisExpr, args);
      case arrObject:
         return new ArrayBuiltin<Dynamic,BUILTIN,CREMENT>(inSrc, thisExpr, args);
      case arrAny:
         return new ArrayBuiltinAny<BUILTIN,CREMENT::OP>(inSrc, thisExpr, args);
      case arrNotArray:
         break;
   }
   throw "Unknown array type";
   return 0;
}



CppiaExpr *createArrayBuiltin(CppiaExpr *src, ArrayType inType, CppiaExpr *inThisExpr, String field,
                              Expressions &ioExpressions )
{
   if (field==HX_CSTRING("concat"))
      return TCreateArrayBuiltin<afConcat,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("copy"))
      return TCreateArrayBuiltin<afCopy,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("insert"))
      return TCreateArrayBuiltin<afInsert,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("iterator"))
      return TCreateArrayBuiltin<afIterator,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("join"))
      return TCreateArrayBuiltin<afJoin,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("pop"))
      return TCreateArrayBuiltin<afPop,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("push"))
      return TCreateArrayBuiltin<afPush,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("remove"))
      return TCreateArrayBuiltin<afRemove,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("reverse"))
      return TCreateArrayBuiltin<afReverse,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("shift"))
      return TCreateArrayBuiltin<afShift,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("splice"))
      return TCreateArrayBuiltin<afSplice,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("slice"))
      return TCreateArrayBuiltin<afSlice,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("sort"))
      return TCreateArrayBuiltin<afSort,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("toString"))
      return TCreateArrayBuiltin<afToString,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("unshift"))
      return TCreateArrayBuiltin<afUnshift,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("map"))
      return TCreateArrayBuiltin<afMap,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("filter"))
      return TCreateArrayBuiltin<afFilter,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("indexOf"))
      return TCreateArrayBuiltin<afIndexOf,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("lastIndexOf"))
      return TCreateArrayBuiltin<afLastIndexOf,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("__get"))
      return TCreateArrayBuiltin<af__get,NoCrement>(src, inType, inThisExpr, ioExpressions);
   if (field==HX_CSTRING("__set"))
      return TCreateArrayBuiltin<af__set,NoCrement>(src, inType, inThisExpr, ioExpressions);

   printf("Bad array field '%s'\n", field.__s);
   throw "Unknown array field";
   return 0;
}



// --- String -------------------------

struct StringExpr : public CppiaExpr
{
   CppiaExpr *strVal;
   StringExpr(CppiaExpr *inSrc, CppiaExpr *inThis )
      : CppiaExpr(inSrc)
   {
      strVal = inThis;
   }
   ExprType getType() { return etString; }
   CppiaExpr *link(CppiaData &inData)
   {
      strVal = strVal->link(inData);
      return this;
   }
   hx::Object *runObject(CppiaCtx *ctx)
   {
      return Dynamic(runString(ctx)).mPtr;
   }
};

template<bool SUBSTR>
struct SubStrExpr : public StringExpr
{
   CppiaExpr *a0;
   CppiaExpr *a1;
   SubStrExpr(CppiaExpr *inSrc, CppiaExpr *inThis, CppiaExpr *inA0, CppiaExpr *inA1)
      : StringExpr(inSrc,inThis)
   {
      a0 = inA0;
      a1 = inA1;
   }
   CppiaExpr *link(CppiaData &inData)
   {
      a0 = a0->link(inData);
      a1 = a1->link(inData);
      return StringExpr::link(inData);
   }
   String runString(CppiaCtx *ctx)
   {
      String val = strVal->runString(ctx);
      int start = a0->runInt(ctx);
      if (SUBSTR)
         return val.substr(start, a1->runObject(ctx) );
      else
         return val.substring(start, a1->runObject(ctx) );
   }
};


template<bool UPPER>
struct ToCaseExpr : public StringExpr
{
   ToCaseExpr(CppiaExpr *inSrc, CppiaExpr *inThis ) : StringExpr(inSrc,inThis) { }
   String runString(CppiaCtx *ctx)
   {
      String val = strVal->runString(ctx);
      if (UPPER)
         return val.toUpperCase();
      else
         return val.toLowerCase();
   }
};

template<bool CODE>
struct CharAtExpr : public StringExpr
{
   CppiaExpr *a0;

   CharAtExpr(CppiaExpr *inSrc, CppiaExpr *inThis, CppiaExpr *inIndex ) : StringExpr(inSrc,inThis)
   {
      a0 = inIndex;
   }
   CppiaExpr *link(CppiaData &inData)
   {
      a0 = a0->link(inData);
      return StringExpr::link(inData);
   }
   ExprType getType() { return CODE ? etObject : etString; }

   String runString(CppiaCtx *ctx)
   {
      String val = strVal->runString(ctx);
      return val.charAt(a0->runInt(ctx));
   }
   int runInt(CppiaCtx *ctx)
   {
      //printf("Char code at %d INT\n", CODE);
      String val = strVal->runString(ctx);
      return val.charCodeAt(a0->runInt(ctx));
   }
   hx::Object *runObject(CppiaCtx *ctx)
   {
      String val = strVal->runString(ctx);

      if (CODE)
         return val.charCodeAt(a0->runInt(ctx)).mPtr;
      else
         return Dynamic(val.charAt(a0->runInt(ctx))).mPtr;
   }
};



struct SplitExpr : public CppiaExpr
{
   CppiaExpr *strVal;
   CppiaExpr *a0;

   SplitExpr(CppiaExpr *inSrc, CppiaExpr *inThis, CppiaExpr *inDelim ) :
      CppiaExpr(inSrc)
   {
      strVal = inThis;
      a0 = inDelim;
   }
   CppiaExpr *link(CppiaData &inData)
   {
      strVal = strVal->link(inData);
      a0 = a0->link(inData);
      return this;
   }
   ExprType getType() { return etObject; }

   hx::Object *runObject(CppiaCtx *ctx)
   {
      String val = strVal->runString(ctx);
      return val.split( a0->runString(ctx) ).mPtr;
   }
};



template<bool LAST>
struct IndexOfExpr : public CppiaExpr
{
   CppiaExpr *strVal;
   CppiaExpr *sought;
   CppiaExpr *start;

   IndexOfExpr(CppiaExpr *inSrc, CppiaExpr *inThis, CppiaExpr *inSought, CppiaExpr *inStart ) :
      CppiaExpr(inSrc)
   {
      strVal = inThis;
      sought = inSought;
      start = inStart;
   }
   ExprType getType() { return etInt; }
   CppiaExpr *link(CppiaData &inData)
   {
      strVal = strVal->link(inData);
      sought = sought->link(inData);
      start = start->link(inData);
      return this;
   }
   int runInt(CppiaCtx *ctx)
   {
      String val = strVal->runString(ctx);
      String s = sought->runString(ctx);
      if (LAST)
         return val.lastIndexOf(s,start->runObject(ctx));
      else
         return val.indexOf(s,start->runObject(ctx));
   }
   hx::Object *runObject(CppiaCtx *ctx) { return Dynamic(runInt(ctx)).mPtr; }
};



// TODO
// static function fromCharCode( code : Int ) : String;


CppiaExpr *createStringBuiltin(CppiaExpr *inSrc, CppiaExpr *inThisExpr, String field, Expressions &ioExpressions )
{
   if (field==HX_CSTRING("toString"))
   {
      if (ioExpressions.size()!=0) throw "Bad arg count";
      return inThisExpr;
   }
   else if (field==HX_CSTRING("toUpperCase"))
   {
      if (ioExpressions.size()!=0) throw "Bad arg count";
      return new ToCaseExpr<true>(inSrc,inThisExpr);
   }
   else if (field==HX_CSTRING("toLowerCase"))
   {
      if (ioExpressions.size()!=0) throw "Bad arg count";
      return new ToCaseExpr<false>(inSrc,inThisExpr);
   }
   else if (field==HX_CSTRING("charAt"))
   {
      if (ioExpressions.size()!=1) throw "Bad arg count";
      return new CharAtExpr<false>(inSrc,inThisExpr,ioExpressions[0]);
   }
   else if (field==HX_CSTRING("cca"))
   {
      if (ioExpressions.size()!=1) throw "Bad arg count";
      return new CharAtExpr<true>(inSrc,inThisExpr,ioExpressions[0]);
   }
   else if (field==HX_CSTRING("charCodeAt"))
   {
      if (ioExpressions.size()!=1) throw "Bad arg count";
      return new CharAtExpr<true>(inSrc,inThisExpr,ioExpressions[0]);
   }
   else if (field==HX_CSTRING("split"))
   {
      if (ioExpressions.size()!=1) throw "Bad arg count";
      return new SplitExpr(inSrc,inThisExpr,ioExpressions[0]);
   }
   else if (field==HX_CSTRING("indexOf"))
   {
      if (ioExpressions.size()!=2) throw "Bad arg count";
      return new IndexOfExpr<false>(inSrc,inThisExpr,ioExpressions[0], ioExpressions[1]);
   }
   else if (field==HX_CSTRING("lastIndexOf"))
   {
      if (ioExpressions.size()!=2) throw "Bad arg count";
      return new IndexOfExpr<true>(inSrc,inThisExpr,ioExpressions[0], ioExpressions[1]);
   }

   else if (field==HX_CSTRING("substr"))
   {
      if (ioExpressions.size()!=2) throw "Bad arg count";
      return new SubStrExpr<true>(inSrc,inThisExpr, ioExpressions[0], ioExpressions[1]);
   }
   else if (field==HX_CSTRING("substring"))
   {
      if (ioExpressions.size()!=2) throw "Bad arg count";
      return new SubStrExpr<false>(inSrc,inThisExpr, ioExpressions[0], ioExpressions[1]);
   }

   return 0;
}


} // end namespace hx
