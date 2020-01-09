/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1999,2008 Oracle.  All rights reserved.
 *
 * $Id: PreparedTransaction.java,v 12.7 2008/01/17 05:04:53 mjc Exp $
 */
package com.sleepycat.db;

import com.sleepycat.db.internal.DbTxn;

/**
The PreparedTransaction object is used to encapsulate a single prepared,
but not yet resolved, transaction.
*/
public class PreparedTransaction {
    private byte[] gid;
    private Transaction txn;

    PreparedTransaction(final DbTxn txn, final byte[] gid) {
        this.txn = new Transaction(txn);
        this.gid = gid;
    }

    public byte[] getGID() {
        return gid;
    }

    /**
    Return the transaction handle for the transaction.
    <p>
    @return
    The transaction handle for the transaction.
    */
    public Transaction getTransaction() {
        return txn;
    }
}
