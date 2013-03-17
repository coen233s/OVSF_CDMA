#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG
#ifdef WIN32
#define dout(x) cout << x
#else
#define dout(x...) cout << x
#endif
#else
#define dout(...)
#endif

#ifdef VERBOSE
#ifdef WIN32
#define vout(x) cout << x
#else
#define vout(x...) cout << x
#endif
#else
#define vout(...)
#endif

#endif /* DEBUG_H_ */
