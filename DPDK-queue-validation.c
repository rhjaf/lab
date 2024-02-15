/*
 * Get the allowed maximum number of RX queues.
 * *pid return the port id which has minimal value of
 * max_rx_queues in all ports.
 */
queueid_t get_allowed_max_nb_rxq(portid_t *pid)
{
        queueid_t allowed_max_rxq = RTE_MAX_QUEUES_PER_PORT;
        bool max_rxq_valid = false;
        portid_t pi;
        struct rte_eth_dev_info dev_info;

        RTE_ETH_FOREACH_DEV(pi) {
                if (eth_dev_info_get_print_err(pi, &dev_info) != 0)
                        continue;

                max_rxq_valid = true;
                if (dev_info.max_rx_queues < allowed_max_rxq) {
                        allowed_max_rxq = dev_info.max_rx_queues;
                        *pid = pi;
                }
        }
        return max_rxq_valid ? allowed_max_rxq : 0;
}

/*
 * Check input rxq is valid or not.
 * If input rxq is not greater than any of maximum number
 * of RX queues of all ports, it is valid.
 * if valid, return 0, else return -1
 */
int check_nb_rxq(queueid_t rxq)
{
        queueid_t allowed_max_rxq;
        portid_t pid = 0;

        allowed_max_rxq = get_allowed_max_nb_rxq(&pid);
        if (rxq > allowed_max_rxq) {
                fprintf(stderr,
                        "Fail: input rxq (%u) can't be greater than max_rx_queues (%u) of port %u\n",
                        rxq, allowed_max_rxq, pid);
                return -1;
        }
        return 0;
}
/*
 * Get the allowed maximum number of TX queues.
 * *pid return the port id which has minimal value of
 * max_tx_queues in all ports.
 */
queueid_t get_allowed_max_nb_txq(portid_t *pid)
{
        queueid_t allowed_max_txq = RTE_MAX_QUEUES_PER_PORT;
        bool max_txq_valid = false;
        portid_t pi;
        struct rte_eth_dev_info dev_info;

        RTE_ETH_FOREACH_DEV(pi) {
                if (eth_dev_info_get_print_err(pi, &dev_info) != 0)
                        continue;

                max_txq_valid = true;
                if (dev_info.max_tx_queues < allowed_max_txq) {
                        allowed_max_txq = dev_info.max_tx_queues;
                        *pid = pi;
                }
        }
        return max_txq_valid ? allowed_max_txq : 0;
}

/*
 * Check input txq is valid or not.
 * If input txq is not greater than any of maximum number
 * of TX queues of all ports, it is valid.
 * if valid, return 0, else return -1
 */
int check_nb_txq(queueid_t txq)
{
        queueid_t allowed_max_txq;
        portid_t pid = 0;

        allowed_max_txq = get_allowed_max_nb_txq(&pid);
        if (txq > allowed_max_txq) {
                fprintf(stderr,
                        "Fail: input txq (%u) can't be greater than max_tx_queues (%u) of port %u\n",
                        txq, allowed_max_txq, pid);
                return -1;
        }
        return 0;
}
