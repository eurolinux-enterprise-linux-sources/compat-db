/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996-2003
 *	Sleepycat Software.  All rights reserved.
 */

#include "db_config.h"

#ifndef lint
static const char revid[] = "$Id: txn_method.c,v 11.66 2003/06/30 17:20:30 bostic Exp $";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#ifdef HAVE_RPC
#include <rpc/rpc.h>
#endif

#include <string.h>
#endif

#include "db_int.h"
#include "dbinc/txn.h"

#ifdef HAVE_RPC
#include "dbinc_auto/db_server.h"
#include "dbinc_auto/rpc_client_ext.h"
#endif

static int __txn_get_tx_max __P((DB_ENV *, u_int32_t *));
static int __txn_get_tx_timestamp __P((DB_ENV *, time_t *));
static int __txn_set_tx_timestamp __P((DB_ENV *, time_t *));

/*
 * __txn_dbenv_create --
 *	Transaction specific initialization of the DB_ENV structure.
 *
 * PUBLIC: void __txn_dbenv_create __P((DB_ENV *));
 */
void
__txn_dbenv_create(dbenv)
	DB_ENV *dbenv;
{
	/*
	 * !!!
	 * Our caller has not yet had the opportunity to reset the panic
	 * state or turn off mutex locking, and so we can neither check
	 * the panic state or acquire a mutex in the DB_ENV create path.
	 */

	dbenv->tx_max = DEF_MAX_TXNS;

#ifdef HAVE_RPC
	if (F_ISSET(dbenv, DB_ENV_RPCCLIENT)) {
		dbenv->get_tx_max = __dbcl_get_tx_max;
		dbenv->set_tx_max = __dbcl_set_tx_max;
		dbenv->get_tx_timestamp = __dbcl_get_tx_timestamp;
		dbenv->set_tx_timestamp = __dbcl_set_tx_timestamp;

		dbenv->txn_checkpoint = __dbcl_txn_checkpoint;
		dbenv->txn_recover = __dbcl_txn_recover;
		dbenv->txn_stat = __dbcl_txn_stat;
		dbenv->txn_begin = __dbcl_txn_begin;
	} else
#endif
	{
		dbenv->get_tx_max = __txn_get_tx_max;
		dbenv->set_tx_max = __txn_set_tx_max;
		dbenv->get_tx_timestamp = __txn_get_tx_timestamp;
		dbenv->set_tx_timestamp = __txn_set_tx_timestamp;

		dbenv->txn_checkpoint = __txn_checkpoint_pp;
		dbenv->txn_recover = __txn_recover_pp;
		dbenv->txn_stat = __txn_stat_pp;
		dbenv->txn_begin = __txn_begin_pp;
	}
}

static int
__txn_get_tx_max(dbenv, tx_maxp)
	DB_ENV *dbenv;
	u_int32_t *tx_maxp;
{
	*tx_maxp = dbenv->tx_max;
	return (0);
}

/*
 * __txn_set_tx_max --
 *	DB_ENV->set_tx_max.
 *
 * PUBLIC: int __txn_set_tx_max __P((DB_ENV *, u_int32_t));
 */
int
__txn_set_tx_max(dbenv, tx_max)
	DB_ENV *dbenv;
	u_int32_t tx_max;
{
	ENV_ILLEGAL_AFTER_OPEN(dbenv, "DB_ENV->set_tx_max");

	dbenv->tx_max = tx_max;
	return (0);
}

static int
__txn_get_tx_timestamp(dbenv, timestamp)
	DB_ENV *dbenv;
	time_t *timestamp;
{
	*timestamp = dbenv->tx_timestamp;
	return (0);
}

/*
 * __txn_set_tx_timestamp --
 *	Set the transaction recovery timestamp.
 */
static int
__txn_set_tx_timestamp(dbenv, timestamp)
	DB_ENV *dbenv;
	time_t *timestamp;
{
	ENV_ILLEGAL_AFTER_OPEN(dbenv, "DB_ENV->set_tx_timestamp");

	dbenv->tx_timestamp = *timestamp;
	return (0);
}
