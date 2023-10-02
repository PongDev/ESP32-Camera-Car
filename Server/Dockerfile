FROM node:alpine AS base

RUN npm i -g pnpm

WORKDIR /app

COPY package.json pnpm-lock.yaml ./

FROM base AS builder

COPY src tsconfig.json /app/
RUN pnpm i
RUN pnpm build

FROM base AS deploy

COPY --from=builder /app/build/ /app/build/
COPY index.html /app/
RUN pnpm i --prod

CMD node build/index.js
