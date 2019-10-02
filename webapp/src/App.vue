<template>
  <div id="app">
    <b-container>

      <b-row class="mb-4" align-v="center">
        <b-col><img src="./assets/logo.png" height="33"></b-col>
        <b-col class="text-right">
          <div v-if="isPending">
            working...
          </div>
          <div v-else-if="account">
            {{ account.name }} &nbsp;&nbsp; <a href="javascript:" class="btn btn-primary" @click="logout()">logout</a>
          </div>
          <div v-else-if="isNetworkConfigEntered">
            <a href="javascript:" class="btn btn-primary" @click="login()">login</a>
          </div>
        </b-col>
      </b-row>

      <b-alert v-if="errorMessage" variant="danger" show>
        {{ errorMessage }}
      </b-alert>
      <b-alert v-if="successMessage" variant="success" show>
        {{ successMessage }}
      </b-alert>
      <b-alert v-if="statusMessage" variant="info" show>
        {{ statusMessage }}
      </b-alert>

      <div v-if="!isConnected && isNetworkConfigEntered === false">
        <b-row class="justify-content-md-center">
          <b-col cols="12" md="auto" class="text-center">
            <h4 class="mb-3">Please Enter Network Params</h4>
            <b-form-group>
              <b-form-input v-model="blockChainEndpoint" placeholder="Network API URL"></b-form-input>
            </b-form-group>
            <b-form-group>
              <b-form-input v-model="tokenSymbol" placeholder="Token Symbol"></b-form-input>
            </b-form-group>
            <b-form-group>
              <b-form-input v-model="tokenPrecision" placeholder="Token Precision"></b-form-input>
            </b-form-group>
            <b-form-group>
              <b-form-input v-model="smartContractAccountName" placeholder="Smart Contract Account Name"></b-form-input>
            </b-form-group>
            <b-button size="lg" text="Button" variant="primary" class="mt-4" :disabled="!!isPending" @click="validateNetworkParamsInput()">
              Connect
            </b-button>
          </b-col>
        </b-row>
      </div>

      <div v-if="isConnected" class="my-4">
        <h4>Add New Record</h4>
        <b-input-group v-if="isConnected" class="mb-3">
          <b-form-input v-model="message" placeholder="Message"></b-form-input>
          <b-input-group-append>
            <b-button size="sm" text="Button" variant="primary" :disabled="!message" @click="logMessage()">
              Log Message
            </b-button>
          </b-input-group-append>
        </b-input-group>
      </div>

      <div v-if="isConnected && isLoadingTableRows && tableRows.length === 0">
        Loading records....
      </div>
      <div v-else-if="tableRows.length > 0" class="my-4">
        <h4>Existing Records</h4>
        <b-table
            :fields="tableFields"
            :items="tableRows"
            :busy="isLoadingTableRows"
            striped
            hover
        >
          <template v-slot:cell(actions)="row">
            <a href="javascript:" @click="deleteRecord(row.item, row.index, $event.target)">
              <font-awesome-icon icon="trash" v-if="row.item.who === account.name" />
            </a>
          </template>
        </b-table>
        <div v-if="hasMoreRows" class="text-center">
          <a href="javascript:" @click="read(true)">load more</a>
        </div>
      </div>
      <div v-else-if="!tableRows.length && isConnected && !isLoadingTableRows" class="my-3">
        Looks like you don't have any records yet. Please use the above input field to add one.
      </div>

    </b-container>
  </div>
</template>

<script>
  import getUrlParam from './services/getUrlParam';
  import ScatterService from './services/scatterService';

  export default {
    name: 'app',
    scatter: ScatterService,
    components: {},
    data: () => ({
      // form
      message: null,

      // scatter
      account: null,

      // contract
      tableFields: ['id', 'who', 'what', 'when', { key: 'actions', label: '' },],
      tableRows: [],
      hasMoreRows: null,

      // messages
      errorMessage: null,
      statusMessage: null,
      successMessage: null,

      // network params
      blockChainEndpoint: null,
      tokenSymbol: null,
      tokenPrecision: null,
      smartContractAccountName: null,

      // statuses
      isNetworkConfigEntered: undefined,
      isConnected: false,
      isPending: false,
      isLoadingTableRows: false,
    }),

    created() {
      this.blockChainEndpoint = getUrlParam('blockchain_endpoint');
      this.tokenSymbol = getUrlParam('token_symbol');
      this.tokenPrecision = getUrlParam('token_precision');
      this.smartContractAccountName = getUrlParam('smart_contract_account_name');

      if (!this.validateNetworkParams()) {
        this.isNetworkConfigEntered = false;
        // this.error('Missing required param');
        return false;
      } else {
        this.isNetworkConfigEntered = true;
      }

      this.connect();
    },

    methods: {

      validateNetworkParamsInput: function () {
        if (this.validateNetworkParams()) {
          this.status('Connecting...');
          this.connect();
        }
        else {
          this.error('Please fill out the form first');
        }
      },

      connect: async function () {
        this.isPending = true;
        try {
          const scatterConfig = {
            api: this.blockChainEndpoint,
            tokenSymbol: this.tokenSymbol,
            tokenPrecision: this.tokenDecimals,
            smartContractAccountName: this.smartContractAccountName,
          };
          const scatter = new ScatterService(scatterConfig);
          await scatter.init();
          this.scatter = scatter;

          await this.login();
        } catch (e) {
          switch (e.message) {
            case 'connection_declined':
              this.error('Please launch Scatter Wallet first and make sure it is unlocked');
              break;
            case 'has_no_account':
              this.error('You do not have an account for this network. Please import your private key in Scatter first');
              break;
            case 'failed_to_connect':
              this.error('Connection failed. Please double check your network connection params.');
              break;
            default:
              this.error(e.message);
          }
        }

        this.isPending = false;
      },

      login: async function () {
        this.isPending = true;
        try {
          this.status('Logging in...');
          const login = await this.scatter.login();
          this.account = login.accounts[0];
          this.isConnected = true;
          this.success(`Logged in as ${this.account.name}`);
          this.loadRecords();
        } catch (e) {
          this.error('You need to allow access in Scatter Wallet to continue');
        }
        this.isPending = false;
      },

      logout: async function (resetMessages = true) {
        this.isPending = true;
        try {
          await this.scatter.logout();
          this.account = null;
          this.isConnected = false;
          if (resetMessages) {
            this.resetMessages();
            this.success('You were successfully logged out');
          }
          this.resetState();
        } catch (e) {
          this.error(e.message);
        }
        this.isPending = false;
      },

      error: function (message) {
        this.resetMessages();
        this.errorMessage = message;
      },

      status: function (message) {
        this.resetMessages();
        this.statusMessage = message;
      },

      success: function (message) {
        this.resetMessages();
        this.successMessage = message;
      },

      resetMessages: function () {
        this.errorMessage = null;
        this.statusMessage = null;
        this.successMessage = null;
      },

      resetState: function () {
        this.tableRows = [];
        this.hasMoreRows = null;
      },

      validateNetworkParams: function () {
        return this.blockChainEndpoint
          && this.tokenSymbol
          && this.tokenPrecision
          && this.smartContractAccountName;
      },

      loadRecords: async function (loadMore = false) {
        this.isLoadingTableRows = true;
        try {
          const lowerBound = loadMore
            ? this.tableRows[this.tableRows.length - 1].id
            : 0;
          const res = await this.scatter.readTable('log', lowerBound);
          this.tableRows = [
            ...(loadMore ? this.tableRows : []),
            ...res.rows,
          ];
          this.hasMoreRows = res.more;
        } catch (e) {
          this.error(e.message);
        }
        this.resetMessages();
        this.isLoadingTableRows = false;
      },

      logMessage: async function () {
        this.status('Logging new message...');
        this.isLoadingTableRows = true;
        try {
          const tx = {
            actions: [
              {
                account: this.smartContractAccountName,
                name: 'createlog',
                authorization: [{
                  actor: this.account.name,
                  permission: this.account.authority,
                }],
                data: {
                  who: this.account.name,
                  what: this.message,
                },
              },
            ],
          };

          const res = await this.scatter.transact(tx);
          this.loadRecords();
          this.message = null;
        } catch (e) {
          this.error(e.message);
        }

        this.isLoadingTableRows = false;
      },

      deleteRecord: async function (item) {
        this.isLoadingTableRows = true;
        const id = item.id;
        this.status(`Deleting record ${id}...`);
        try {
          const tx = {
            actions: [
              {
                account: this.smartContractAccountName,
                name: 'deletelog',
                authorization: [{
                  actor: this.account.name,
                  permission: this.account.authority,
                }],
                data: {
                  who: this.account.name,
                  log_id: id,
                },
              },
            ],
          };

          await this.scatter.transact(tx);
          this.success(`Deleted record ${id}`);
          this.loadRecords();
        } catch (e) {
          this.error("Sorry, you can't delete other people's logs.");
        }
        this.isLoadingTableRows = false;
      },
    }
    ,
  }
</script>

<style>
  #app {
    font-family: 'Avenir', Helvetica, Arial, sans-serif;
    margin-top: 60px;
  }
</style>
