import ScatterJS from '@scatterjs/core';
import ScatterEOS from '@scatterjs/eosjs2';
import { Api, JsonRpc } from 'eosjs';

export default class ScatterService {

  config;

  identity;

  network;

  rpc;

  ready = false;

  constructor(config) {
    this.config = config;
    ScatterJS.plugins(new ScatterEOS());
  }

  async init() {
    try {
      await this.initNetwork();
      const connected = await this.connect();
      if (connected) {
        if (!await this.hasAccount()) {
          throw new Error('has_no_account');
        }
      }
      else {
        throw new Error('connection_declined');
      }
    } catch (e) {
      if (e.message === 'Failed to fetch') {
        throw new Error('failed_to_connect');
      }
      else {
        throw e;
      }
    }
  }

  async initNetwork() {
    this.rpc = new JsonRpc(this.config.api);
    const info = await this.rpc.get_info();

    const apiArr = this.config.api.split('://');
    const proto = apiArr[0];

    this.network = ScatterJS.Network.fromJson({
      blockchain: 'eos',
      chainId: info.chain_id,
      host: apiArr[1],
      port: proto === 'https' ? 443 : 80,
      protocol: proto,
      token: {
        blockchain: 'eos',
        contract: 'eosio.token',
        symbol: this.config.tokenSymbol,
        name: this.config.tokenSymbol,
        decimals: this.config.tokenDecimals,
        chainId: ''
      }
    });

  }

  async connect() {
    try {
      return await ScatterJS.connect('Log Something', {
        network: this.network
      });
    } catch (error) {
      throw error;
    }
  }

  async login() {
    if (typeof ScatterJS.login === 'function') {
      try {
        await ScatterJS.suggestNetwork(this.network);
        this.identity = await ScatterJS.login();
        this.ready = !!this.identity;
        return this.identity;
      } catch (e) {
        this.ready = false;
        throw e;
      }
    }
    else {
      this.ready = false;
      throw new Error('no_scatter');
    }
  }

  async logout() {
    await ScatterJS.logout();
    this.ready = false;
    return true;
  }

  async hasAccount() {
    await ScatterJS.suggestNetwork(this.network);
    return await ScatterJS.hasAccountFor(this.network);
  }

  async readTable(table, upperBound = null, limit = 10) {
    let query = {
      json: true,
      code: this.config.smartContractAccountName,
      scope: this.config.smartContractAccountName,
      reverse: true,
      show_payer: false,
      table,
      limit,
    };

    if (upperBound) {
      query.upper_bound = upperBound;
    }

    return await this.rpc.get_table_rows(query);
  }

  async transact(trx) {
    const eos = ScatterJS.eos(this.network, Api, { rpc: this.rpc });
    return eos.transact(trx, {
      blocksBehind: 3,
      expireSeconds: 30
    });
  }
}
