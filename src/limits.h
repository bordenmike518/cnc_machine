#ifndef LIMITS_H_
#define LIMITS_H_

/* Setup of limit switches */
void limits_init(void);

/* Returns true if any end stop is set to HIGH */
bool limits_get(void);

#endif // LIMITS_H_
