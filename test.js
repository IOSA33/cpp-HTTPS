// Usage "k6 run test.js"

import http from 'k6/http';

export const options = {
    insecureSkipTLSVerify: true,
    vus: 350,
    duration: '30s',
};

export default function () {
    http.get('https://localhost:6788/json');
}